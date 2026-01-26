#include <Arduino.h>
#include "secrets.h"

const float standardVoltage = 100;
float currentVoltage = 400;

/**
 * @brief Retrieve air quality from air quality sensor
 * @return Int : air quality level in %
 * @param None
*/
float getAirQual() {
    int lastVoltage = currentVoltage;
    currentVoltage = analogRead(AIR_QUAL_SENSOR_PIN);

    Serial.print("Actual air quality (%): ");
    Serial.print(currentVoltage);

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

    return currentVoltage;
}
