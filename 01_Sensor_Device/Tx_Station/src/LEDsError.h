#include <Arduino.h>
#include "secrets.h"


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

void ErrorTempHumLED()
{
    for(int i = 0; i < 3; i++)
    {
        setColor(255, 0, 0);
        delay(500);
        setColor(0, 0, 0);
        delay(500);
    }
}