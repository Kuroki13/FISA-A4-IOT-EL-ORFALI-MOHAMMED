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

// Topics
const char* TOPIC_DATA = "eclss/salle1/pression";

// Timer pour envoi non-bloquant
unsigned long lastMsgTime = 0;
const long interval = 2000; // Envoi toutes les 2 secondes


void reconnect() {
  // Boucle jusqu'à connexion
  while (!mqttClient.connected()) {
    Serial.print("Connexion MQTT...");
    String clientId = "ECLSS_Client_";
    
    if (mqttClient.connect(clientId.c_str())) {
      Serial.println("Connecte !");
      // On peut envoyer un message de bonjour
      mqttClient.publish("eclss/status", " ECLSS en ligne");
    } else {
      Serial.print("Echec, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" nouvel essai dans 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(9600);
  delay(2000);

  // Initialisation du capteur de pression
  initCapteurPression();
  
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

  if (!mqttClient.connected()){
	reconnect();
  }

  // Garder MQTT actif
  mqttClient.loop();

  // Envoie sans délai
  unsigned long now = millis();
  if (now - lastMsgTime > interval) {
	lastMsgTime = now;

  // Lecture de la pression actuelle
  float pression = getDataPression();

  // Affichage debug

  Serial.print("Envoi pression : ");
  Serial.print(pression);
  Serial.println("hPa");
  Serial.println("---------------------");

  // 3. Création du Payload JSON
    // Format : {"valeur": 1013.25, "unit": "hPa"}
    String payload = "{\"valeur\":";
    payload += String(pression, 2); // 2 décimales
    payload += ", \"unit\":\"hPa\"}";

    // 4. Envoi MQTT
    Serial.print("Envoi MQTT: ");
    Serial.println(payload);
    mqttClient.publish(TOPIC_DATA, payload.c_str());

}
}