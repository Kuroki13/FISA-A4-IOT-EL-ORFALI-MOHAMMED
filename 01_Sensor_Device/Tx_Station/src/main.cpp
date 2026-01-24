#include <Arduino.h>
#include <WiFiS3.h>
#include <PubSubClient.h>
#include "Sensor_Pressure.h"
#include "Sensor_TempHum.h"

const char *ssid = "";
const char *password = "";

const char *mqtt_server = "192.168.0.157";
const uint16_t mqtt_port = 1883;
bool isMqttConnected = false;

// Topics
const char *TOPIC_PRESSION = "eclss/pression";
const char *TOPIC_TEMPERATURE = "eclss/temperature";
const char *TOPIC_HUMIDITE = "eclss/humidite";

uint8_t MAC_ADDRESS[6];
char STR_MAC_ADDRESS[18];

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup()
{
	Serial.begin(9600);
	delay(1000);
	initBme280();

	Serial.println("=== UNO R4 #2 : Client MQTT ===");

	// Connexion WiFi
	int status = WL_IDLE_STATUS;
	while (status != WL_CONNECTED)
	{
		Serial.print("Connexion à ");
		Serial.println(ssid);

		status = WiFi.begin(ssid, password);
		Serial.print("status=");
		Serial.println(status);
		delay(3000);
	}

	Serial.println("WiFi connecté ! IP : " + WiFi.localIP().toString());

	WiFi.macAddress(MAC_ADDRESS);
	snprintf(STR_MAC_ADDRESS, sizeof(STR_MAC_ADDRESS),"%02X:%02X:%02X:%02X:%02X:%02X",MAC_ADDRESS[0], MAC_ADDRESS[1], MAC_ADDRESS[2], MAC_ADDRESS[3], MAC_ADDRESS[4], MAC_ADDRESS[5]);

	Serial.print("Device ID (MAC): ");
	Serial.println(STR_MAC_ADDRESS);
	
	// Configuration MQTT
	mqttClient.setServer(mqtt_server, mqtt_port);
}

void connexionMQTT()
{

	char clientId[23];
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
		sendMQTTMessage(getDataPression(), TOPIC_PRESSION);
		sendMQTTMessage(getTemp(), TOPIC_TEMPERATURE);
		sendMQTTMessage(getHum(), TOPIC_HUMIDITE);
		Serial.println("------------------------");
		delay(1000);
	}
}