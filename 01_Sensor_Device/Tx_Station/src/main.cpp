#include <Arduino.h>
#include <WiFiS3.h>
#include <PubSubClient.h>
#include <ArduinoBearSSL.h>
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

void computeHMAC(const char* message, char* outputHex)
{
	uint8_t hmac[32]; // SHA-256 = 32 bytes

	br_hmac_context ctx;
	br_hmac_key_context keyCtx;

	br_hmac_key_init(&keyCtx, &br_sha256_vtable, HMAC_KEY, strlen(HMAC_KEY));
	br_hmac_init(&ctx, &keyCtx, 0);
	br_hmac_update(&ctx, message, strlen(message));
	br_hmac_out(&ctx, hmac);

	// conversion hex
	for (int i = 0; i < 32; i++){
		sprintf(outputHex + (i * 2), "%02x", hmac[i]);
	}
	outputHex[64] = '\0';
}


void sendMQTTMessage(float data, const char *topic)
{
	char dataStr[10];
	dtostrf(data, 0, 2, dataStr);

	// chaîne à signer
	char signBuffer[64];
	snprintf(signBuffer, sizeof(signBuffer),"id=%s&data=%s", STR_MAC_ADDRESS, dataStr);

	// HMAC
	char hmacHex[65];
	computeHMAC(signBuffer, hmacHex);

	// payload final
	char payload[160];
	snprintf(payload, sizeof(payload),
		"{\"id\":\"%s\",\"data\":%s,\"hmac\":\"%s\"}",
		STR_MAC_ADDRESS, dataStr, hmacHex);

	mqttClient.publish(topic, payload);

	Serial.println("MQTT envoyé :");
	Serial.println(payload);
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