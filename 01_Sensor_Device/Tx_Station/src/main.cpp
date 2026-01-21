#include <Arduino.h>
#include <WiFiS3.h>
#include <PubSubClient.h>
#include "Sensor_Pressure.h"


const char* ssid     = "R4-GRP1_AP";
const char* password = "12345678";

const char* mqtt_server = "192.168.4.67";
const uint16_t mqtt_port = 1883;
bool mqttconnexion = false;


// Topics
const char* TOPIC_DATA = "eclss/salle1/pression";

// Timer pour envoi non-bloquant
unsigned long lastMsgTime = 0;
const long interval = 2000; // Envoi toutes les 2 secondes

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(9600);
  delay(2000);
  
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

bool connexionMQTT(){
	
  String clientId = "UNO_R4_Client";
  if (mqttClient.connect(clientId.c_str())) {
    Serial.println("MQTT connecté !");
  } else {
    Serial.println("MQTT échec");
	return false;
  }
}

void sendMqttMessage(){
	// Envoi test sur topic "test"
	mqttClient.publish(TOPIC_DATA, "imldkjmlqjfd");
	mqttClient.disconnect();
	Serial.println("Message '21' envoyé sur topic 'test'");
  }

  
void sendPressure(){

	// Garder MQTT actif
	mqttClient.loop();

	// Envoie sans délai
	unsigned long now = millis();
	if (now - lastMsgTime > interval) {
		lastMsgTime = now;

		// Lecture de la pression actuelle
		float pression = getDataPression();
		
		char buffer[10];  // Taille suffisante pour "1234.56\0"
		dtostrf(pression, 6, 2, buffer);  // 6 chiffres total, 2 décimales
		Serial.print("Envoi MQTT: ");
		Serial.println(buffer);
		mqttClient.publish(TOPIC_DATA, buffer);

	}
}




void loop() {
	if(connexionMQTT()){
		sendPressure();
	}
    delay(1000);
    Serial.println("Ping " + WiFi.localIP().toString());
}
