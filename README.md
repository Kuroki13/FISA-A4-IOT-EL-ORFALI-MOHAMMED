#  Projet IoT : "Un Campus Qui Vous Comprend"

![Status](https://img.shields.io/badge/Status-Development-yellow?style=flat-square) 
![Platform](https://img.shields.io/badge/Hardware-Arduino%20%7C%20ESP32-blue?style=flat-square) 
![Middleware](https://img.shields.io/badge/Middleware-NodeRED%20%7C%20MQTT-green?style=flat-square)
![License](https://img.shields.io/badge/License-MIT-lightgrey?style=flat-square)


> **Groupe 1 :** Développement et qualification d'un sous-système de contrôle environnemental (ECLSS) pour station spatiale.
>
> **Objectif :** Réalisation et validation d'un **Modèle d'Ingénierie (EM)** en environnement analogue (Salle de classe).

---

##  L'Équipe (Flight Crew & Ground Control)

| Membre | Rôle Technique | Responsabilités Mission |
| :--- | :--- | :--- |
| **Mohammed EL ORFALI** | Dev. Embarqué | Développement Firmware Capteur & Algorithmes de survie (Safety) |
| **Axel DELAR** | Ingénieur Radio | Protocoles de transmission & Validation Couverture Wi-Fi/4G |
| **Remy MOREEL** | Architecte Réseau | Architecture du *Flight Computer* (Gateway) & Gestion flux de données |
| **Léo SCHAEFFER** | Intégrateur Cloud | Développement du *Mission Control* (Dashboard) & Stockage |

---

## Contexte & Problématique : "La Classe comme Station"

![Mission Status](https://img.shields.io/badge/Mission-Active-green?style=for-the-badge) 
![System](https://img.shields.io/badge/System-Life_Support-red?style=for-the-badge)
![Criticality](https://img.shields.io/badge/Criticality-High-orange?style=for-the-badge)

Dans une station spatiale (ISS), l'air est une ressource critique. Une augmentation du CO2 ou une baisse de pression n'est pas un simple inconfort, c'est une question de survie immédiate. Les délais de communication avec la Terre interdisent une gestion de crise 100% Cloud.

**Notre approche : Le "Module Pressurisé Autonome"**
Nous utilisons une salle de classe comme **environnement analogue** (volume clos, présence humaine, génération de CO2) pour qualifier un système capable de :

1.  **Survie (Edge Computing) :** Le module gère sa propre sécurité. Si le CO2 dépasse le seuil critique, l'alarme sonne **localement** et **instantanément**, sans attendre Internet.
2.  **Résilience (Store & Forward) :** En cas de perte de signal (LOS - Loss of Signal) avec le Cloud, les données sont stockées dans la Gateway et retransmises au rétablissement de la liaison.

---

## Architecture Technique (Modèle d'Ingénierie)

Le système suit une architecture IoT en couches stricte, adaptée aux contraintes spatiales simulées.

<img width="4015" height="1878" alt="image" src="https://github.com/user-attachments/assets/94f7f5d5-38d6-4a26-8277-12e3b0061e2c" />

### 1. Zone de Perception (Smart Sensor)
Le "Nez" du système. Il est autonome énergétiquement et décisionnellement.
* **Hardware :** Arduino (Avec ESP32) sur secteur & Powerbank (Simulation Batterie Secours).
* **Capteurs Critique :**
    * Qualité d'air : Grove Gas Sensor (MQ135 / Ref 101020078).
    * Environnement : Capteur Temp/Humidité (AM2302).
* **Sécurité :** Algorithme réflexe `IF CO2 > 1200ppm THEN Trigger_Alarm()`.
* **Protocole :** MQTT sur Wi-Fi local.

### 2. Zone Middleware (Flight Computer)
Le cerveau local du module.
* **Hardware :** PC Hôte.
* **Rôle Gateway :** Héberge le Broker MQTT (Mosquitto) et le moteur de flux (Node-RED).
* **Fonction Store & Forward :**
    * *Status OK :* Passthrough vers le Cloud.
    * *Status LOS (Coupure) :* Mise en mémoire tampon (Buffer local).

### 3. Zone Application (Mission Control)
La supervision distante pour les ingénieurs au sol.
* **Interface :** Dashboard Node-RED / Grafana.
* **Données :** Télémétrie temps réel + Historique des incidents.

---

## Interface de Données (API)

Le module transmet sa télémétrie toutes les 5 secondes sur le topic `iss/module_1/eclss/telemetry`.

**Format du Payload JSON :**
```json
{
  "device_id": "sensor_unit_alpha",
  "timestamp": 1705421200,
  "telemetry": {
    "co2_ppm": 1150,
    "temperature_c": 21.5,
    "humidity_pct": 45.0,
    "door_status": "CLOSED"
  },
  "system_status": {
    "battery_level": 85,
    "wifi_rssi": -65,
    "alarm_active": false
  }
