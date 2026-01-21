#include <Arduino.h>
#include <WiFiS3.h>
#include <PubSubClient.h>
#include "Sensor_Pressure.h"



const char* ssid     = "R4-GRP1_AP";
const char* password = "12345678";

const char* mqtt_server = "192.168.4.2";
const uint16_t mqtt_port = 1883;
const char* mqtt_topic  = "test";

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
  
  // Connexion MQTT
  String clientId = "UNO_R4_Client";
  if (mqttClient.connect(clientId.c_str())) {
    Serial.println("MQTT connecté !");
    
    // Envoi test sur topic "test"
    mqttClient.publish(mqtt_topic, "zobi");
    mqttClient.disconnect();
    Serial.println("Message '21' envoyé sur topic 'test'");
  } else {
    Serial.println("MQTT échec");
  }
}

void loop() {
  delay(10000);
  Serial.println("Ping " + WiFi.localIP().toString());
  float pression = getDataPression();
}