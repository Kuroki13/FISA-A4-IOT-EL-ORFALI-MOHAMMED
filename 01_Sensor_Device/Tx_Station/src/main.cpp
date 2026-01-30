#include <Arduino.h>
#include <WiFiS3.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>
#include "Sensors.h"
#include "secrets.h"

bool isMqttConnected = false;

uint8_t MAC_ADDRESS[6];
char STR_MAC_ADDRESS[18];

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
LiquidCrystal_I2C lcd(LCD_SCREEN_I2C,16,2);

void setup()
{
	Serial.begin(9600);
	while (!Serial);
	Serial.println("------------------------");
	initBme280();
	lcd.init();
	lcd.noBacklight();
	ErrorLEDsInit();

	// Connexion WiFi
	int status = WL_IDLE_STATUS;
	while (status != WL_CONNECTED)
	{
		Serial.print("Connexion to ");
		Serial.println(WIFI_SSID);

		status = WiFi.begin(WIFI_SSID, WIFI_PASS);
		Serial.print("Status = ");
		Serial.println(status);
		if(status != WL_CONNECTED) {
			Serial.println("WiFi not connected, check the modem!");
			delay(5000);
			Serial.println("Trying to reconnect to the WiFi ...");
		}
	}

	while (WiFi.localIP() == INADDR_NONE) delay(100);

	Serial.println("WiFi connected!");
	Serial.println("IP : " + WiFi.localIP().toString());

	WiFi.macAddress(MAC_ADDRESS);
	snprintf(STR_MAC_ADDRESS, sizeof(STR_MAC_ADDRESS),"%02X:%02X:%02X:%02X:%02X:%02X",MAC_ADDRESS[0], MAC_ADDRESS[1], MAC_ADDRESS[2], MAC_ADDRESS[3], MAC_ADDRESS[4], MAC_ADDRESS[5]);

	Serial.print("Device ID (MAC): ");
	Serial.println(STR_MAC_ADDRESS);
	
	// Configuration MQTT
	mqttClient.setServer(MQTT_SRV_ID, MQTT_SRV_PORT);
}

void connexionMQTT()
{

	char clientId[32];
	snprintf(clientId,sizeof(clientId),"UNO_R4_Client_%s}",STR_MAC_ADDRESS);

	if (mqttClient.connect(clientId))
	{
		Serial.println("MQTT connecté !");
		Serial.println(clientId);
		isMqttConnected = true;
	}
	else
	{
		Serial.println("MQTT échec");
		isMqttConnected = false;
		delay(5000);
	}
}

void sendMQTTMessage(float data, const char *topic)
{
	char message[10];
	dtostrf(data, 6, 2, message);

	char payload[128];
	snprintf(payload,sizeof(payload),"{\"id\":\"%s\",\"data\":%s}",STR_MAC_ADDRESS,message);

	Serial.println(payload);
	
	// Envoi sur le topic spécifié
	mqttClient.publish(topic, payload);

	Serial.print("Message '");
	Serial.print(message);
	Serial.print("' envoyé sur topic ");
	Serial.print(topic);
	Serial.print(" depuis l'arduino ");
	Serial.println(STR_MAC_ADDRESS);
}

void loop()
{
	if (!isMqttConnected)
	{
		connexionMQTT();
	}
	mqttClient.loop();
	if (isMqttConnected)
	{
		lcd.backlight();
		Serial.println("------------------------");

		float temp = getTemp();
		lcd.setCursor(0,0);
		lcd.print(temp);
		lcd.setCursor(5,0);
		lcd.print("C");

		float hum = getHum();
		lcd.setCursor(9,0);
		lcd.print(hum);
		lcd.setCursor(14,0);
		lcd.print("%H");

		float press = getPress();
		lcd.setCursor(0,1);
		String printPress = String((int) press);
		while (printPress.length() < 4) printPress = " " + printPress;
		lcd.print(printPress);
		lcd.setCursor(4,1);
		lcd.print("hPa");

		float airQual = getAirQual();
		lcd.setCursor(9,1);
		lcd.print(airQual);
		lcd.setCursor(14,1);
		lcd.print("%A");


		sendMQTTMessage(temp, TOPIC_TEMPERATURE);
		sendMQTTMessage(hum, TOPIC_HUMIDITE);
		sendMQTTMessage(press, TOPIC_PRESSION);
		sendMQTTMessage(airQual, TOPIC_AIR_QUAL);

		// SERIAL
		Serial.print("Actual temperature (°C): ");
		Serial.println(temp);
		Serial.print("Actual humidity (%): ");
    	Serial.println(hum);
		Serial.print("Actual pressure (hPa): ");
		Serial.println(press);
		delay(5000);
	}
}