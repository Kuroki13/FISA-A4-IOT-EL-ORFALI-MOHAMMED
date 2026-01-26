#include <Arduino.h>
#include <WiFiS3.h>
#include <PubSubClient.h>
#include "Sensor_Pressure.h"
#include "Sensor_TempHum.h"
#include "Sensor_AirQual.h"
#include "secrets.h"

bool isMqttConnected = false;

uint8_t MAC_ADDRESS[6];
char STR_MAC_ADDRESS[18];

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup()
{
	Serial.begin(9600);
	while (!Serial);
	Serial.println("------------------------");
	initBme280();

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
		Serial.println("------------------------");
		sendMQTTMessage(getTemp(), TOPIC_TEMPERATURE);
		sendMQTTMessage(getHum(), TOPIC_HUMIDITE);
		sendMQTTMessage(getPress(), TOPIC_PRESSION);
		sendMQTTMessage(getAirQual(), TOPIC_AIR_QUAL);
		delay(5000);
	}
}