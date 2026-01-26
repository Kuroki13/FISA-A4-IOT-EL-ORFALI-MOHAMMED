#include <Adafruit_BME280.h>
#include <Arduino.h>

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
    status = bme.begin(0x76);
    if (!status)
    {
        while (!status)
        {
            Serial.println("Could not find a valid BME280 sensor, check wiring!");
            Serial.print("SensorID was: 0x");
            Serial.println(bme.sensorID(), 16);
            delay(5000);
            Serial.println("Trying to reconnect to the BME280 sensor ...");
            status = bme.begin(0x76);
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
    Serial.print("Actual temperature (°C): ");
    Serial.println(temp);
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
    Serial.print("Actual humidity (%): ");
    Serial.println(hum);
    return hum;
}
