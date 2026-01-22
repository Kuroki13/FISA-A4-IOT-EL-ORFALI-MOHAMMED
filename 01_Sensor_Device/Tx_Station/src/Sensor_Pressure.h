#ifndef SENSOR_PRESSURE_H
#define SENSOR_PRESSURE_H



#include <Arduino.h>

// Configuration du capteur pression
const int pressureSensorPin = A0; // Pin analogique pour le capteur de pression

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
 * Pour le SPX3058D, il faudra ajuster les valeurs min/max.
 */

 float convertirEnPression(int valeurBrute){

	// --- FORMULE REELLE (A calibrer avec un baromètre de référence) ---
  	// Exemple théorique : 0.5V = 0 hPa, 4.5V = 2000 hPa
  	// Sur Arduino R4 (ADC 10-bit par défaut) :
  	// Voltage = (valeurBrute / 1023.0) * 5.0;
  	// Pression = (Voltage - Offset) * ScaleFactor;

  	// Pour une station spatiale, on veut mesurer de 0 (Vide) à ~1200 hPa (Max sécurité)
  	float hpa = (valeurBrute / 1023.0) * 1200.0; 
  	return hpa;
 }





 
 void initCapteurPression() {
	Serial.begin(9600);
	pinMode(pressureSensorPin, INPUT);
	

	
}

float getDataPression(){
	float raw = 0.0;

	// Acquisition & Filtrage
	raw = lireValeurBrutePression();
	// Conversion
	pressionActuelle = convertirEnPression(raw);
	Serial.print("Pression actuelle (hPa): ");
	Serial.println(pressionActuelle);
	


	
	return pressionActuelle;

	delay(1000);

}

#endif // SENSOR_PRESSURE_H 