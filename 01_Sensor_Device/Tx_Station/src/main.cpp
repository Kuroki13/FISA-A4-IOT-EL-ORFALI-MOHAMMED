#include <Arduino.h>
#include <WiFiS3.h>
#include <PubSubClient.h>
#include "Sensor_Pressure.h"


const char* ssid     = "R4-GRP1_AP";
const char* password = "12345678";

const char* mqtt_server = "192.168.4.2";
const uint16_t mqtt_port = 1883;
bool isMqttConnected = false;


// Topics
const char* PRESSURE_TOPIC = "eclss/salle1/pression";

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
	Serial.begin(9600);
	delay(1000);
	
	Serial.println("=== UNO R4 #2 : Client MQTT ===");
	
	// Connexion WiFi
	int status = WL_IDLE_STATUS;
	while (status != WL_CONNECTED) {
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

void connexionMQTT(){
	
  String clientId = "UNO_R4_Client";
  if (mqttClient.connect(clientId.c_str())) {
    Serial.println("MQTT connecté !");
	isMqttConnected = true;
  } else {
    Serial.println("MQTT échec");
	isMqttConnected = false;
  }
}

void sendMqttMessage(char* message, const char* topic){
	// Envoi test sur topic "test"
	mqttClient.publish(topic, message);
	
	Serial.print("Message '");
	Serial.print(message);
	Serial.print("' envoyé sur topic ");
	Serial.println(topic);
  }


char* getCleanPressure(){ //a déplacer dans Sensor_pressure.h à la fin de la fonction getDataPression /!\ ne pas oublié de changer le type de retour
    float pression = getDataPression();
    static char buffer[10];
    dtostrf(pression, 6, 2, buffer);
    return buffer;
}

void loop() {
    if (!isMqttConnected) {
        connexionMQTT();
    }
    mqttClient.loop();
    if (isMqttConnected) {
        char* pressure = getCleanPressure();
        sendMqttMessage(pressure, PRESSURE_TOPIC);
        delay(1000);
    }
}