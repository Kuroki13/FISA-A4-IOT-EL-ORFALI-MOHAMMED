#include <Adafruit_BME280.h>
#include <Arduino.h>
#include "LEDsError.h"

///////////////////////////////////////////
// TEMPERATURE AND HUMIDITY SENSOR (BME280)
///////////////////////////////////////////

Adafruit_BME280 bme;

/**
 * @brief Inititalize the BME280 sensor 
 * @return None
 * @param None
*/
void initBme280()
{
    unsigned status;
    // Initialisation capteur BME280 et vérification connexion
    status = bme.begin(BME280_SENSOR_I2C);
    if (!status)
    {
        while (!status)
        {
            Serial.println("Could not find a valid BME280 sensor, check wiring!");
            Serial.print("SensorID was: 0x");
            Serial.println(bme.sensorID(), 16);
            delay(5000);
            Serial.println("Trying to reconnect to the BME280 sensor ...");
            status = bme.begin(BME280_SENSOR_I2C);
        }
    }
    Serial.println("BME280 sensor connected!");
}

/**
 * @brief Retrieve temperature from BME280 sensor
 * @return Float : temperature level in C°
 * @param None
*/
float getTemp()
{
    float temp = bme.readTemperature();
    return temp;
}

/**
 * @brief Retrieve humidity from BME280 sensor
 * @return Float : humidity level in %
 * @param None
*/
float getHum()
{
    float hum = bme.readHumidity();
    return hum;
}


///////////////////////////////////////////
// PRESSURE SENSOR
///////////////////////////////////////////

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
 * @return Float : pressure level in hPa
 * @param None
*/
float getPress()
{
	float valeurBrute = lireValeurBrutePression();
	// Conversion (simulateValue / step) * maxNeeded;
	float pressionActuelle = (valeurBrute / 1023.0) * 1200.0;

	return pressionActuelle;
}


///////////////////////////////////////////
// AIR QUALITY SENSOR
///////////////////////////////////////////

const float standardVoltage = 100;
float currentVoltage = 400;

/**
 * @brief Retrieve air quality from air quality sensor
 * @return Float : air quality level in %
 * @param None
*/
float getAirQual() {
    float lastVoltage = currentVoltage;
    currentVoltage = analogRead(AIR_QUAL_SENSOR_PIN);
    float percentVoltage = (currentVoltage/1024) * 100;

    Serial.print("Actual air quality (%): ");
    Serial.print(percentVoltage);

    if (currentVoltage - lastVoltage > 400 || currentVoltage > 700) {
        Serial.println(". High pollution! Force signal active.");
    }
    else if ((currentVoltage - lastVoltage > 400 && currentVoltage < 700) || currentVoltage - standardVoltage > 150) {
        Serial.println(". High pollution!");
    }
    else if (( currentVoltage - lastVoltage > 200 && currentVoltage < 700) || currentVoltage - standardVoltage > 50) {
        Serial.println(". Low pollution!");
    }
    else {
        Serial.println(". Fresh air.");
    }

    return percentVoltage;
}
