#include <Arduino.h>

#define PIN_DIRECTION_RIGHT 3
#define PIN_DIRECTION_LEFT 4
#define PIN_MOTOR_PWM_RIGHT 5
#define PIN_MOTOR_PWM_LEFT 6

#define I2C_ADDRESS 0x20
#define LEDS_COUNT 10

#define PIN_BATTERY A0
#define PIN_BUZZER A0

#define PIN_SONIC_TRIG 7
#define PIN_SONIC_ECHO 8
#define MAX_DISTANCE 300
#define SONIC_TIMEOUT (MAX_DISTANCE * 60)
#define SOUND_VELOCITY 340

#define PIN_SERVO 2
#define MILIEU_SERVO 90
#define GAUCHE_SERVO 170
#define DROITE_SERVO 10

long TEMPS;
float DISTANCE;

// #include <Servo.h>
// Servo servo;

#include <Freenove_WS2812B_RGBLED_Controller.h>
Freenove_WS2812B_Controller strip(I2C_ADDRESS, LEDS_COUNT, TYPE_GRB);

#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
RF24 radio(9, 10);
const byte adress[6] = "00001";
String direcJoy;

struct structure3
{
	int VitesseG;
	int VitesseD;
	int VitesseG_Bas;
	int VitesseD_Bas;
	bool Direction;
	bool Pile_gauche;
	bool Pile_droite;
	int Vitesse_pile_gauche;
	int vitesse_pile_droite;
	bool klaxon;
	char BoutonDirection;

	int mode = 1;

	float colorSend;

	bool blink;
};
structure3 Data;

bool BuzzerModeUn = 0;
bool BuzzerModeDeux = 0;

long prevMillis;
bool blinkTempo;

void initSetup()
{
	pinMode(PIN_SONIC_TRIG, OUTPUT);
	pinMode(PIN_SONIC_ECHO, INPUT);

	// servo.attach(PIN_SERVO);
	// servo.write(MILIEU_SERVO);

	//Servo désactivé car il faisait des bruit bizzard avec les leds, réactiver si besoin

	Serial.begin(9600);
	while (!Serial);

	pinMode(PIN_DIRECTION_RIGHT, OUTPUT);
	pinMode(PIN_DIRECTION_LEFT, OUTPUT);
	pinMode(PIN_MOTOR_PWM_RIGHT, OUTPUT);
	pinMode(PIN_MOTOR_PWM_LEFT, OUTPUT);

	// pinMode(PIN_BATTERY, INPUT);
	pinMode(PIN_BUZZER, OUTPUT);

	while (!strip.begin());
	strip.setAllLedsColor(0x000000);

	radio.begin();
	radio.setChannel(125);
	radio.openReadingPipe(1, adress);
	radio.startListening();
}

float Sonar()
{
	digitalWrite(PIN_SONIC_TRIG, HIGH);
	delayMicroseconds(10);
	digitalWrite(PIN_SONIC_TRIG, LOW);

	TEMPS = pulseIn(PIN_SONIC_ECHO, HIGH, SONIC_TIMEOUT);

	if (TEMPS != 0)
	{
		DISTANCE = float(TEMPS * SOUND_VELOCITY / 2 / 10000);
	}
	else
	{
		DISTANCE = MAX_DISTANCE;
	}

	return DISTANCE;
}

float getBatteryVoltage()
{
	pinMode(PIN_BATTERY, INPUT);

	int batteryADC = analogRead(PIN_BATTERY);

	if (batteryADC < 614)
	{
		float batteryVoltage = batteryADC / 1023.0 * 5.0 * 4;
		return batteryVoltage;
	}

	else
	{
		// le buzzer est activé
	}

	return 0; // obliger de return quelque chose mais ça sert a rien
}

void SOS(float vitesseSOS)
{
	for (int i = 0; i < 3; i++)
	{
		digitalWrite(PIN_BUZZER, HIGH);
		delay(vitesseSOS);
		digitalWrite(PIN_BUZZER, LOW);
		delay(vitesseSOS);
	}
	delay(vitesseSOS * 2);
	for (int i = 0; i < 3; i++)
	{
		digitalWrite(PIN_BUZZER, HIGH);
		delay(vitesseSOS * 2);
		digitalWrite(PIN_BUZZER, LOW);
		delay(vitesseSOS * 2);
	}
	delay(vitesseSOS * 2);
	for (int i = 0; i < 3; i++)
	{
		digitalWrite(PIN_BUZZER, HIGH);
		delay(vitesseSOS);
		digitalWrite(PIN_BUZZER, LOW);
		delay(vitesseSOS);
	}
}

void ApparaitreColorLed(int r, int g, int b, int pause) // 100
{
	for (int i = 0; i < LEDS_COUNT; i++)
	{
		strip.setLedColor(i, r, g, b);
		delay(pause);
	}
}

void Rainbow()
{
	for (int j = 0; j < 255; j += 2)
	{
		for (int i = 0; i < LEDS_COUNT; i++)
		{
			strip.setLedColorData(i, strip.Wheel((i * 256 / LEDS_COUNT + j) & 255));
		}
		strip.show();
		delay(2);
	}
}

void moteur(String direction, int vitesseG, int vitesseD)
{
	if (direction == "haut")
	{
		digitalWrite(PIN_DIRECTION_LEFT, LOW);
		digitalWrite(PIN_DIRECTION_RIGHT, HIGH);
	}
	else if (direction == "bas")
	{
		digitalWrite(PIN_DIRECTION_LEFT, HIGH);
		digitalWrite(PIN_DIRECTION_RIGHT, LOW);
	}
	else if (direction == "gauche")
	{
		digitalWrite(PIN_DIRECTION_LEFT, HIGH);
		digitalWrite(PIN_DIRECTION_RIGHT, HIGH);
	}
	else if (direction == "droite")
	{
		digitalWrite(PIN_DIRECTION_LEFT, LOW);
		digitalWrite(PIN_DIRECTION_RIGHT, LOW);
	}
	else if (direction == "stop")
	{
		// rien à faire
	}

	analogWrite(PIN_MOTOR_PWM_LEFT, vitesseG);
	analogWrite(PIN_MOTOR_PWM_RIGHT, vitesseD);
}

void moteurStop()
{
	analogWrite(PIN_MOTOR_PWM_RIGHT, 0);
	analogWrite(PIN_MOTOR_PWM_LEFT, 0);
}