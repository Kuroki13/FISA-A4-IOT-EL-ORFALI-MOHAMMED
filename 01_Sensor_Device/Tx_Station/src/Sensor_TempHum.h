#include <Adafruit_BME280.h>
#include <Arduino.h>

Adafruit_BME280 bme;

void initBme280()
{
    unsigned status;
    // Initialisation capteur BME280 et vérification connexion
    status = bme.begin(0x76);
    if (!status)
    {
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        Serial.print("SensorID was: 0x");
        Serial.println(bme.sensorID(), 16);
        while (1)
            delay(10);
    }
}

float getTemp()
{
    Serial.print("Température actuelle (°C): ");
    Serial.println(bme.readTemperature());
    return bme.readTemperature();
}

float getHum()
{
    Serial.print("Humidité actuelle (%): ");
    Serial.println(bme.readHumidity());
    return bme.readHumidity();
}
