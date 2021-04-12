#include <variable.h>

void setup()
{
	Serial.begin(9600);

	radio.begin();
	radio.setChannel(125);
	radio.openWritingPipe(adress);

	for (int i = 0; i < 7; i++)
	{
		pinMode(Bouton[i], INPUT_PULLUP);
	}
}

void loop()
{
	verifBtJoy();

	if (Etat.BtF == 0)
	{
		Data.mode = 1;
	}

	if (Etat.BtE == 0)
	{
		Data.mode = 2;
	}

	// Algorytme pour transfomer l'état du JoyStick en 2 variable qui sont ensuite envoyé a la voiture
	Direction.HAUT = map(Etat.JoyY, 515, 1023, 0, 255);
	Direction.BAS = map(Etat.JoyY, 0, 515, 255, 0);

	if (Direction.HAUT > 0)
	{
		Direction.DROITE = map(Etat.JoyX, 523, 1023, Direction.HAUT, 0);
		Direction.GAUCHE = map(Etat.JoyX, 0, 523, 0, Direction.HAUT);
	}
	else if (Direction.BAS > 0)
	{
		Direction.DROITE = map(Etat.JoyX, 523, 1023, Direction.BAS, 0);
		Direction.GAUCHE = map(Etat.JoyX, 0, 523, 0, Direction.BAS);

		Direction.DROITE_BAS = map(Etat.JoyX, 523, 1023, Direction.BAS, 0);
		Direction.GAUCHE_BAS = map(Etat.JoyX, 0, 523, 0, Direction.BAS);
	}
	else if (Direction.HAUT == 0 && Direction.BAS == 0)
	{
		Direction.GAUCHE = 0;
		Direction.DROITE = 0;
		Direction.DROITE_BAS = 0;
		Direction.GAUCHE_BAS = 0;
	}

	if (Direction.GAUCHE > 255)
	{
		Direction.GAUCHE = 255;
	}
	if (Direction.DROITE > 255)
	{
		Direction.DROITE = 255;
	}

	if (Direction.GAUCHE_BAS > 255)
	{
		Direction.GAUCHE_BAS = 255;
	}
	if (Direction.DROITE_BAS > 255)
	{
		Direction.DROITE_BAS = 255;
	}

	Data.VitesseD = Direction.DROITE;
	Data.VitesseG = Direction.GAUCHE;

	Data.VitesseG_Bas = Direction.GAUCHE_BAS;
	Data.VitesseD_Bas = Direction.DROITE_BAS;

	if (Data.VitesseD < 85)
	{
		Data.VitesseD = 0;
	}
	if (Data.VitesseG < 85)
	{
		Data.VitesseG = 0;
	}

	if (Data.VitesseD_Bas < 85)
	{
		Data.VitesseD_Bas = 0;
	}
	if (Data.VitesseG_Bas < 85)
	{
		Data.VitesseG_Bas = 0;
	}

	if (Direction.HAUT > 0)
	{
		Data.Direction = 0; // Avant
	}
	else
	{
		Data.Direction = 1; // Arrière
	}

	// Ajout d'un mode pour que si Y est au centre et qu'on fait varier X, que l'effet de tourner soit plus fluide
	Data.Vitesse_pile_gauche = map(Etat.JoyX, 0, 523, 255, 0);
	Data.vitesse_pile_droite = map(Etat.JoyX, 523, 1023, 0, 255);

	if (Data.Vitesse_pile_gauche < 85)
	{
		Data.Vitesse_pile_gauche = 0;
	}
	if (Data.vitesse_pile_droite < 85)
	{
		Data.vitesse_pile_droite = 0;
	}

	if (Etat.JoyY == 515 && Etat.JoyX > 523)
	{
		Data.Pile_droite = 1;
	}
	else
	{
		Data.Pile_droite = 0;
	}

	if (Etat.JoyY == 515 && Etat.JoyX < 523)
	{
		Data.Pile_gauche = 1;
	}
	else
	{
		Data.Pile_gauche = 0;
	}

	// Klaxon quand tu appuie surn le joystick
	Data.klaxon = Etat.BtK;

	// Bouger la voiture avec les flèches directionnelles
	// Faire bouger le servomoteur dans le sousmode 2 (1 variable pour 2 foonctions, si c'est pas de l'opti ça :-)

	if (Etat.BtA == 0)
	{
		Data.BoutonDirection = 'h';
	}
	else if (Etat.BtB == 0)
	{
		Data.BoutonDirection = 'd';
	}
	else if (Etat.BtC == 0)
	{
		Data.BoutonDirection = 'b';
	}
	else if (Etat.BtD == 0)
	{
		Data.BoutonDirection = 'g';
	}
	else
	{
		Data.BoutonDirection = 0;
	}

	// Pour le mode couleur

	zoneX = map(Etat.JoyX, 0, 1020, 0, 16);
	zoneY = map(Etat.JoyY, 0, 1020, 16, 0);

	if (Etat.BtC == 0 && BtTempoC == 1)
	{
		BtTempoC = 0;
		EtatLed = !EtatLed;
		decouverte = 1;
	}

	if (Etat.BtC == 1 && BtTempoC == 0)
	{
		BtTempoC = 1;
	}

	if (EtatLed == 1)
	{

		if (Etat.BtB == 0 && BtTempoB == 1)
		{
			BtTempoB = 0;
			decouverte = 0;
		}
		if (Etat.BtB == 1 && BtTempoB == 0)
		{
			BtTempoB = 1;
		}

		if (Etat.BtD == 0 && BtTempoD == 1)
		{
			BtTempoD = 0;
			decouverte = 1;
		}
		if (Etat.BtD == 1 && BtTempoD == 0)
		{
			BtTempoD = 1;
		}

		if (decouverte == 1)
		{
			Data.colorSend = table_color[zoneY][zoneX];
		}

		if (Etat.BtA == 0 && BtTempoBlink == 1)
		{
			BtTempoBlink = 0;
			Data.blink = !Data.blink;

			if (Data.blink == 0)
			{
				decouverte = 1;
			}
			
		}
		else if (Etat.BtA == 1 && BtTempoBlink == 0)
		{
			BtTempoBlink = 1;
		}
	}
	else
	{
		Data.colorSend = 0x000000;
	}

	// Envoyer les données à la voiture
	radio.write(&Data, sizeof(structure3));
}
