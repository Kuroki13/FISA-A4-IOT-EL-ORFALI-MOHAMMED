#  Projet IoT : "Un Campus Qui Vous Comprend"

![Status](https://img.shields.io/badge/Status-Development-yellow?style=flat-square) 
![Platform](https://img.shields.io/badge/Hardware-Arduino%20%7C%20ESP32-blue?style=flat-square) 
![Middleware](https://img.shields.io/badge/Middleware-NodeRED%20%7C%20MQTT-green?style=flat-square)


> **Groupe 1 :** Développement et qualification d'un sous-système de contrôle environnemental (ECLSS) pour station spatiale.
>
> **Objectif :** Réalisation et validation d'un **Modèle d'Ingénierie (EM)** en environnement analogue (Salle de classe).

---

##  L'Équipe

| Membre | Rôle Technique | Responsabilités Mission |
| :--- | :--- | :--- |
| **Mohammed EL ORFALI** | Dev. Embarqué | Développement Firmware Capteur & Algorithmes de survie (Safety) |
| **Axel DELAR** | Ingénieur Radio | Protocoles de transmission & Validation Couverture Wi-Fi/4G |
| **Remy MOREEL** | Architecte Réseau | Architecture du (Gateway) & Gestion flux de données |
| **Léo SCHAEFFER** | Intégrateur Cloud | Développement du (Dashboard) & Stockage |

---

## Contexte & Problématique : "La Classe comme Station"

![Mission Status](https://img.shields.io/badge/Mission-Active-green?style=for-the-badge) 
![System](https://img.shields.io/badge/System-Life_Support-red?style=for-the-badge)
![Criticality](https://img.shields.io/badge/Criticality-High-orange?style=for-the-badge)

Dans une station spatiale (ISS), l'air est une ressource critique. Une augmentation du CO2 ou une baisse de pression n'est pas un simple inconfort, c'est une question de survie immédiate. Les délais de communication avec la Terre interdisent une gestion de crise 100% Cloud.

**Notre approche : Le "Module Pressurisé Autonome"**
Nous utilisons une salle de classe comme **environnement analogue** (volume clos, génération de CO2) pour qualifier un système capable de :

1.  **Survie (Edge Computing) :** Le module gère sa propre sécurité. Si le CO2 dépasse le seuil critique, l'alarme sonne **localement** et **instantanément**, sans attendre Internet.

---

## Architecture Technique (Modèle d'Ingénierie)

Le système suit une architecture IoT en couches stricte, adaptée aux contraintes spatiales simulées.

<img width="4015" height="1878" alt="image" src="https://github.com/user-attachments/assets/94f7f5d5-38d6-4a26-8277-12e3b0061e2c" />

### 1. Zone de Perception
Le "Nez" du système. Il est autonome énergétiquement et décisionnellement.
* **Hardware :** Arduino (Avec ESP32) sur secteur & Powerbank (Simulation Batterie Secours).
* **Capteurs Critique :**
    * Qualité d'air : Grove Gas Sensor (MQ135 / Ref 101020078).
    * Environnement : Capteur Temp/Humidité (AM2302).
* **Sécurité :** Algorithme réflexe `IF CO2 > 1200ppm THEN Trigger_Alarm()`.
* **Protocole :** MQTT sur Wi-Fi local.

### 2. Zone Middleware
Le cerveau local du module.
* **Hardware :** PC Hôte.
* **Rôle Gateway :** Héberge le Broker MQTT (Mosquitto) et le moteur de flux (Node-RED).

### 3. Zone Application
La supervision distante pour les ingénieurs au sol.
* **Interface :** Dashboard Node-RED.
* **Données :** Télémétrie temps réel + Historique des incidents.

---

## Interface de Données

Le module transmet sa télémétrie toutes les 5 secondes sur le topic `eclss/télémetrie`.

**Format du Payload JSON :**
```json
{
        "id": "821ed53125562099",
        "type": "tab",
        "label": "GRAPHIQUES",
        "disabled": false,
        "info": "",
        "env": []
    },
    {
        "id": "c7015096d238237f",
        "type": "group",
        "z": "821ed53125562099",
        "name": "GRAPHIQUE SALLE 1",
        "style": {
            "stroke": "#000000",
            "fill": "#d1d1d1",
            "label": true,
            "color": "#000000"
        },
        "nodes": [
            "ec4d7a4e82cce2f2",
            "88a6b0668f27c40c",
            "658985ec2a1d724f",
            "656f1342826a0567",
            "46b9011460992d86",
            "f8d7d4595bd604d4",
            "945a7696eff37191"
        ],
        "x": 284,
        "y": 119,
        "w": 362,
        "h": 242
    }
}
