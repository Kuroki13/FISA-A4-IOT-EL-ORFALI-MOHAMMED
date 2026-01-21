#ifndef SENSOR_PRESSURE_H
#define SENSOR_PRESSURE_H



#include <Arduino.h>

// Configuration du capteur pression
const int pressureSensorPin = A0; // Pin analogique pour le capteur de pression
const bool mode_Simulation = false; // true pour mode simulation, false pour mode réel

//-- Simulation --
// Si true : La pression change toute seule (vague)
// Si false : Tu tapes la pression dans le moniteur série
const bool mode_Auto=true; 


// Seuille de sécurité (ECLSS) pour la pression (en hPa)
const float PRESSION_NOMINALE = 1013.25; // Pression standard au niveau de la mer
const float PRESSION_SEUIL_BAS = 980.0;  // Seuil bas de pression (fuite possible)
const float PRESSION_SEUIL_HAUT = 1050.0; // Seuil haut de pression (surpression possible)


// Variable globales
float pressionActuelle = 0.0;

// -- Simulation -- Variable globale qui stocke notre "Pression Virtuelle"
float pressionSimulee = PRESSION_NOMINALE;

bool alarmeActivee = false;


 // Les fonctions de gestion de la pression 

 /** --Simulation--
  * 0. SIMULATION DE PRESSION (optionnel)
  * Si on est en mode simulation, on fait varier la pression simulée.
  * Sinon, on lit la pression depuis le capteur.
  */

  void GenererPressionSimulee(){
	if(mode_Auto){
		// OPTION 1 : Simulation Automatique (Sinusoïde)
    // Fait varier la pression entre 900 et 1100 hPa toutes les 10 secondes
    // Utilise le temps (millis) pour générer une onde

	float temps = millis() / 1000.0; // Temps en secondes
	pressionSimulee = 1000.0 + (100.0 * sin(temps)); // Variation sinusoidale

	// Ajout d'un peu de "bruit" aléatoire pour faire réaliste (+/- 1 hPa)
    pressionSimulee += random(-100, 100) / 100.0;

	} else {
		// OPTION 2 : Simulation Manuelle (Moniteur Série)
		if (Serial.available() > 0){
			float valeurLue = Serial.parseFloat();
			// Serial.parseFloat() retourne 0.0 s'il ne comprend pas ou lit juste "Entrée"
      		// On ignore donc les 0.0 sauf si on veux vraiment simuler le vide spatial
			if (valeurLue >= 10.0){
				pressionSimulee = valeurLue;
				Serial.print(">>> COMMANDE RECUE : Pression forcée à ");
				Serial.print(pressionSimulee);
				Serial.println(" hPa");
			}

			// Nettoyer le buffer série
			while (Serial.available() > 0) {
				Serial.read();
			}
		}

	};
 }

 /**
 * 1. ACQUISITION & FILTRAGE
 * Lit le capteur X fois et fait une moyenne pour supprimer le bruit.
 */

 int lireValeurBrutePression(){
	long somme = 0;
	int nombreEchantillons = 10; // On fait la moyenne de 10 lectures

	for (int i = 0; i < nombreEchantillons; i++){
		somme += analogRead(pressureSensorPin);
		delay(2); 
	}

	return somme / nombreEchantillons;
 }

 /**
 * 2. CONVERSION (Calibration)
 * Transforme la valeur 0-1023 en hPa.
 * Note : Pour le SPX3058D, c'est ici qu'il faudra ajuster les valeurs min/max.
 */

 float convertirEnPression(int valeurBrute){
	if(mode_Simulation){
		// Si on simule, on map 0-1023 vers 900-1100 hPa pour tester les alarmes
		return map(valeurBrute, 0, 1023, 900, 1100);
	}

	// --- FORMULE REELLE (A calibrer avec un baromètre de référence) ---
  	// Exemple théorique : 0.5V = 0 hPa, 4.5V = 2000 hPa
  	// Sur Arduino R4 (ADC 10-bit par défaut) :
  	// Voltage = (valeurBrute / 1023.0) * 5.0;
  	// Pression = (Voltage - Offset) * ScaleFactor;

	// On va utiliser une mapping linéaire simple pour le test :
  	// Supposons que le capteur sort entre 0 et 1023 pour 0 à 2000 hPa
  	float hpa = (valeurBrute / 1023.0) * 2000.0; 
  	return hpa;
 }

 /**
 * 3. LOGIQUE DE SECURITE
 * Vérifie si on est en danger.
 */

 void verifierSecurite(float pression){
	if (pression < PRESSION_SEUIL_BAS){
		if (!alarmeActivee){
			Serial.println("!!! ALERTE : DEPRESSURISATION DETECTEE !!!");
			alarmeActivee = true;
		}
	} else if (pression > PRESSION_SEUIL_HAUT){
		if (!alarmeActivee){
			Serial.println("!!! ALERTE : SURPRESSION DETECTEE !!!");
			alarmeActivee = true;
		}
	} else {
		if (alarmeActivee){
			Serial.println("Pression revenue à la normale. Alarme désactivée.");
			alarmeActivee = false;
		}
		

	}


 }
 
 void initCapteurPression() {
	Serial.begin(9600);
	pinMode(pressureSensorPin, INPUT);
	Serial.println("--- Capteur Pression Initialisé ---");

	if (!mode_Simulation){
		Serial.println("Mode Réel Activé : Lecture depuis le capteur de pression.");
	} else {
		Serial.println("Mode Simulation Activé : Génération de pression virtuelle.");
		if(mode_Auto){
			Serial.println(" - Simulation Automatique (Sinusoïde)");
		} else {
			Serial.println(" - Simulation Manuelle (Moniteur Série)");
			Serial.println("   -> Entrez une valeur de pression (hPa) et appuyez sur Entrée.");
		}
	}
}

float getDataPression(){
	float raw = 0.0;
	if (mode_Auto){
		GenererPressionSimulee();
		pressionActuelle = pressionSimulee;
		Serial.print(">>> PRESSION SIMULEE : ");
		Serial.println(pressionActuelle);
	} else {
		// Acquisition & Filtrage
	int raw = lireValeurBrutePression();
	// Conversion
	pressionActuelle = convertirEnPression(raw);
	Serial.print(">>> PRESSION MESUREE : ");
	Serial.println(pressionActuelle);
	}

	//Vérification de la sécurité
	verifierSecurite(pressionActuelle);

	// Télémétrie (Debug)
	Serial.print("Valeur Brute : ");
	Serial.print(raw);
	Serial.print(" | Pression: ");
	Serial.print(pressionActuelle);
	Serial.println(" hPa");
	return pressionActuelle;

	delay(1000);

}

#endif // SENSOR_PRESSURE_H 