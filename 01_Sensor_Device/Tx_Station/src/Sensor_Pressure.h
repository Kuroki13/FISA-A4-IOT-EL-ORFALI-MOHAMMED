#include <Arduino.h>
#include "secrets.h"

/**
 * @brief Calculate 
 * @return Int : 
 * @param None
*/
int lireValeurBrutePression()
{
	long somme = 0;
	int nombreEchantillons = 10; // On fait la moyenne de 10 lectures

	for (int i = 0; i < nombreEchantillons; i++)
	{
		somme += analogRead(PRESSURE_SENSOR_PIN);
		delay(2);
	}

	return somme / nombreEchantillons;
}

/**
 * @brief Simulate pressure with potentiometer 
 * @return Float : pressure level in Pa
 * @param None
*/
float getPress()
{
	float valeurBrute = lireValeurBrutePression();
	// Conversion (simulateValue / step) * maxNeeded;
	float pressionActuelle = (valeurBrute / 1023.0) * 1200.0;
	Serial.print("Pression actuelle (hPa): ");
	Serial.println(pressionActuelle);

	return pressionActuelle;
}