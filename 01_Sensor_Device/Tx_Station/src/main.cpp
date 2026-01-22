#include <Arduino.h>
#include <WiFiS3.h>
#include <PubSubClient.h>
#include "Sensor_Pressure.h"
#include "Sensor_TempHum.h"

const char *ssid = "";
const char *password = "";

const char *mqtt_server = "";
const uint16_t mqtt_port = 1883;
bool isMqttConnected = false;

// Topics
const char *TOPIC_PRESSION = "eclss/salle1/pression";
const char *TOPIC_TEMPERATURE = "eclss/salle1/temperature";
const char *TOPIC_HUMIDITE = "eclss/salle1/humidite";

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

	// Configuration MQTT
	mqttClient.setServer(mqtt_server, mqtt_port);
}

void connexionMQTT()
{

	String clientId = "UNO_R4_Client";
	if (mqttClient.connect(clientId.c_str()))
	{
		Serial.println("MQTT connecté !");
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

	// Envoi sur le topic spécifié
	mqttClient.publish(topic, message);

	Serial.print("Message '");
	Serial.print(message);
	Serial.print("' envoyé sur topic ");
	Serial.println(topic);
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