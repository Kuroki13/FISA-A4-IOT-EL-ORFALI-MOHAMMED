#include <Arduino.h>
#include <WiFiS3.h>
#include <PubSubClient.h>
#include <Adafruit_BME280.h>

const char* ssid = "";
const char* password = "";

const char* mqtt_server = "";
const uint16_t mqtt_port = 1883;
bool mqttconnexion = false;

Adafruit_BME280 bme;

// Topic
const char* TOPIC_DATA_TEST = "eclss/salle1/test";
const char* TOPIC_DATA_TEMP = "eclss/salle1/temperature";
const char* TOPIC_DATA_HUM = "eclss/salle1/humidite";

// Timer pour envoi non-bloquant
unsigned long lastMsgTime = 0;
// Envoi toutes les 2 secondes
const long interval = 2000;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void setup() {
  Serial.begin(9600);
  Serial.println("=== SETUP DEMARRE ===");

    unsigned status;

    // Initialisation capteur BME280 et vérification connexion
    status = bme.begin(0x76);
    if(!status){
        Serial.println("Could not find a valid BME280 sensor, check wiring!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        while (1) delay(10);
    }

    int statusW = WL_IDLE_STATUS;
    while (statusW != WL_CONNECTED) {
        Serial.print("Connexion à ");
        Serial.println(ssid);

        statusW = WiFi.begin(ssid, password);
        Serial.print("status=");
        Serial.println(statusW);
        delay(3000);
    }
    Serial.println("WiFi connecté ! IP : " + WiFi.localIP().toString());
  
    // Configuration MQTT
    mqttClient.setServer(mqtt_server, mqtt_port);
}

bool connexionMQTT() {
    if (mqttClient.connected()) return true;

    if (mqttClient.connect("UNO_R4_TX_Salle1")) {
        Serial.println("MQTT connecté !");
        return true;
    } else {
        Serial.print("MQTT échec, rc=");
        Serial.println(mqttClient.state());
        return false;
    }
}

void sendMqttMessage(const char* topic, const char* topicValue){
    mqttClient.publish(topic, topicValue);
}

void sendTempHum() {
    if (!mqttClient.connected()) {
        Serial.println("MQTT non connecté, publish annulé");
        return;
  }

    unsigned long now = millis();
    if (now - lastMsgTime > interval) {
        lastMsgTime = now;

        float temp = bme.readTemperature();
        float hum = bme.readHumidity();

        // Conversion des float en string
        char tempStr[10];
        char humStr[10];

        // Utilisation de dtostrf pour convertir float en string
        dtostrf(temp, 4, 2, tempStr);
        dtostrf(hum, 4, 2, humStr);

        sendMqttMessage(TOPIC_DATA_TEMP, tempStr);
        sendMqttMessage(TOPIC_DATA_HUM, humStr);
        Serial.println("Température envoyée: " + String(tempStr) + " °C, Humidité envoyée: " + String(humStr) + " %");
  }
}

void loop() {
    if (!mqttClient.connected()) {
        connexionMQTT();
    }

    mqttClient.loop();

    sendTempHum();

    delay(2000);
}
