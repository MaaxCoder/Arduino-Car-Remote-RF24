#include <variable.h>

void setup()
{
	initSetup();
}

void loop()
{
	if (radio.available())
	{
		radio.read(&Data, sizeof(structure3));

		if (Data.mode == 1)
		{
			if (BuzzerModeUn == 0)
			{
				digitalWrite(PIN_BUZZER, LOW);
				delay(500);
				digitalWrite(PIN_BUZZER, HIGH);
				delay(500);
				digitalWrite(PIN_BUZZER, LOW);
				delay(500);
				BuzzerModeUn = 1;
				BuzzerModeDeux = 0;
			}

			// Seulent si aucune flèche dirrectionnelle n'est utilisé
			if (Data.BoutonDirection == 0)
			{

				// Algorytme pour faire bouger les roues + roues si axe y est au centre (mode sports)

				if (Data.Direction == 1)
				{
					direcJoy = "bas";
				}
				else if (Data.Direction == 0)
				{
					direcJoy = "haut";
				}

				if (Data.Pile_gauche == 1)
				{
					moteur("gauche", Data.Vitesse_pile_gauche, Data.Vitesse_pile_gauche);
				}
				else if (Data.Pile_droite == 1)
				{
					moteur("droite", Data.vitesse_pile_droite, Data.vitesse_pile_droite);
				}
				else
				{
					moteur(direcJoy, Data.VitesseG, Data.VitesseD);
				}
			}

			// Klaxon
			if (Data.klaxon == 0)
			{
				digitalWrite(PIN_BUZZER, HIGH);
			}
			else
			{
				digitalWrite(PIN_BUZZER, LOW);
			}

			if (Data.VitesseG == 0 && Data.VitesseD == 0 && Data.Pile_droite == 0 && Data.Pile_gauche == 0)
			{
				if (Data.BoutonDirection == 'h')
				{
					moteur("haut", 200, 200);
				}
				else if (Data.BoutonDirection == 'b')
				{
					moteur("bas", 200, 200);
				}
				else if (Data.BoutonDirection == 'g')
				{
					moteur("gauche", 200, 200);
				}
				else if (Data.BoutonDirection == 'd')
				{
					moteur("droite", 200, 200);
				}
			}
		}
	}

	if (Data.mode == 2)
	{

		if (BuzzerModeDeux == 0)
		{
			digitalWrite(PIN_BUZZER, LOW);
			delay(500);
			digitalWrite(PIN_BUZZER, HIGH);
			delay(500);
			digitalWrite(PIN_BUZZER, LOW);
			delay(500);
			digitalWrite(PIN_BUZZER, HIGH);
			delay(500);
			digitalWrite(PIN_BUZZER, LOW);
			delay(500);
			BuzzerModeUn = 0;
			BuzzerModeDeux = 1;
		}
		
		if (Data.blink == 0)
		{
			strip.setAllLedsColor(Data.colorSend);
		}
		
		else if(millis() - prevMillis > 200)
		{
			prevMillis = millis();
			
			blinkTempo = !blinkTempo;

			if (blinkTempo == 1)
			{
				strip.setAllLedsColor(Data.colorSend);
			}
			else
			{
				strip.setAllLedsColor(0x000000);
			}
			
			
		}
	}
}