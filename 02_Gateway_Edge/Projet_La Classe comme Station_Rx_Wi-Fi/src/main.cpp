/*
 * ARDUINO ROUTEUR (Access Point)
 * Rôle : Créer le réseau WiFi pour la classe
 */
#include <Arduino.h>
#include <WiFiS3.h>

const char* ssid     = "R4-GRP1_AP";
const char* password = "12345678"; 

WiFiServer server(80);

void setup() {
    Serial.begin(9600);
    delay(1000);

    Serial.println("\n--- Démarrage du ROUTEUR ---");
    
    // Vérification du module WiFi
    if (WiFi.status() == WL_NO_MODULE) {
        Serial.println("Erreur : Pas de module WiFi !");
        while (true);
    }

    // Création du Point d'Accès
    Serial.print("Création du réseau : ");
    Serial.println(ssid);
    
    // Démarrage de l'AP
    auto status = WiFi.beginAP(ssid, password);
    
    if (status != WL_AP_LISTENING) {
        Serial.println("Echec de création de l'AP (Vérifiez le mot de passe !)");
        while (true);
    }

    Serial.println("AP Créé avec succès !");
    Serial.print("Adresse IP du Routeur : ");
    Serial.println(WiFi.localIP()); 

    server.begin();
}

void loop() {
    // code serveur pour répondre aux clients
    WiFiClient client = server.available();
    if (client) {
        Serial.println("Nouveau client connecté !");
        String currentLine = "";
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                // ( code de réponse HTTP)
                 if (c == '\n' && currentLine.length() == 0) {
                    client.println("HTTP/1.1 200 OK");
                    client.println("Content-type:text/html");
                    client.println();
                    client.println("<h1>Bienvenue sur le Routeur GRP1</h1>");
                    client.println();
                    break;
                }
                if (c == '\n') currentLine = "";
                else if (c != '\r') currentLine += c;
            }
        }
        client.stop();
        Serial.println("Client déconnecté");
    }
}