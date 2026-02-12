#include <Adafruit_BME280.h>
#include <Arduino.h>
#include "secrets.h"

///////////////////////////////////////////
// TEMPERATURE AND HUMIDITY SENSOR (BME280)
///////////////////////////////////////////

Adafruit_BME280 bme;

bool errorTempHum = false;
bool errorPress   = false;
bool errorAir     = false;

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

    // Error checking
    if (isnan(temp) || temp <= 0 || temp >= 180)
    {
        errorTempHum = true;
        return 503; // valeur fallback
    }

    errorTempHum = false;
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


    // Error checking
    if (isnan(hum) || hum <= 0 || hum >= 100)
    {
        errorTempHum = true;
        return 0;
    }

    errorTempHum = false;
    return hum;
}


///////////////////////////////////////////
// PRESSURE SENSOR
///////////////////////////////////////////


// /**
//  * @brief Inititalize the pressure sensor 
//  * @return None
//  * @param None
// */

// // --- Variables de configuration ---
// // Alpha : 0.1 = Lissage fort (courbe très douce), 1.0 = Brut (très nerveux)
// const float FILTER_ALPHA = 0.1;
// float filteredPressure = 1013.0; 

// /**
//  * @brief Initialize the pressure sensor
//  */
// void InitPression()
// {
//     pinMode(PRESSURE_SENSOR_PIN, INPUT);
//     int raw = analogRead(PRESSURE_SENSOR_PIN);
//     filteredPressure = (raw / 1023.0) * 1200.0;
// }

// /**
//  * @brief Retrieve pressure with Anti-Ghosting & Exponential Smoothing
//  * @return float : Stable pressure level in hPa
//  */
// float getPress()
// {
//     int sensorValue = analogRead(PRESSURE_SENSOR_PIN);
    
//     float rawHPa = (sensorValue / 1023.0) * 1200.0; 

//     if (rawHPa < 900 || rawHPa > 1170)
//     {
//         errorPress = true;
// 		filteredPressure = (FILTER_ALPHA * rawHPa) + ((1.0 - FILTER_ALPHA) * filteredPressure);
// 		return filteredPressure; 
//     }
//     errorPress = false;

//     // Formule : NouvelleMoyenne = (10% de la Nouveauté) + (90% de l'Historique)
//     filteredPressure = (FILTER_ALPHA * rawHPa) + ((1.0 - FILTER_ALPHA) * filteredPressure);

//     return filteredPressure;
// }


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
    float press = (valeurBrute / 1024.0) * 1200.0;

    // Error checking
    if (press < 900 || press > 1170)
    {
        errorPress = true;
        return press;
    }

	if (press <= 100)
	{
		errorPress = true;
		return 0;
	}

    errorPress = false;
    return press;
}


///////////////////////////////////////////
// AIR QUALITY SENSOR
///////////////////////////////////////////

// const float FILTER_ALPHA_AIR = 0.05; 
// float filteredAirQuality = 0.0; 
// const float standardVoltage = 100;
// float currentVoltage = 400;

// /**
//  * @brief Initialize the air quality sensor
//  */
// void InitAirQual()
// {
//     analogRead(AIR_QUAL_SENSOR_PIN); 
//     delay(2);
//     filteredAirQuality = analogRead(AIR_QUAL_SENSOR_PIN);
// }

// /**
//  * @brief Retrieve air quality from air quality sensor
//  * @return Float : air quality level in %
//  * @param None
// */
// float getAirQual() {

//     currentVoltage = analogRead(AIR_QUAL_SENSOR_PIN);
    


//     // Error checking
//     if (currentVoltage < 0 || currentVoltage > 1023)
//     {
//         errorAir = true;
//         return filteredAirQuality;
//     }

//     errorAir = false;	
// 	filteredAirQuality = (FILTER_ALPHA_AIR * currentVoltage) + ((1.0 - FILTER_ALPHA_AIR) * filteredAirQuality);

// 	float percentVoltage = (filteredAirQuality/1024) * 100;

//     return filteredAirQuality;
// }

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

    // Error checking
    if (currentVoltage < 0 || currentVoltage > 1023)
    {
        errorAir = true;
        return 0;
    }

    errorAir = false;
    return currentVoltage;
}


///////////////////////////////////////////
// ERROR LEDS FUNCTIONS
////////////////////////////////////////////

inline void ErrorLEDsInit()
{
    pinMode(RED_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);
    pinMode(YELLOW_PIN, OUTPUT);
}

void setColor(uint8_t red, uint8_t blue, uint8_t yellow)
{
    analogWrite(RED_PIN, red);
    analogWrite(BLUE_PIN, blue);
    analogWrite(YELLOW_PIN, yellow);
}

void CheckErrorsLED(int statuus){
    if(errorTempHum){
		setColor(255, 0, 0);
        tone(BUZZER_PIN, 440, 500);
	} else if (errorPress){
		setColor(0, 255, 0);
        tone(BUZZER_PIN, 440, 500);
	} else if (errorAir){
		setColor(255, 255, 0);
        tone(BUZZER_PIN, 440, 500);
	} else if(statuus != WL_CONNECTED){
        setColor(255, 0, 0);
        tone(BUZZER_PIN, 440, 10000);
    }
    else {
		setColor(0, 0, 255);
        noTone(BUZZER_PIN);
	}
}

