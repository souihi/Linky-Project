# ESP32 Linky vers ThingsBoard/ThingSpeak

Ce projet permet de lire les données de consommation électrique d'un compteur communiquant français **Linky** (en mode Historique) à l'aide d'un microcontrôleur **ESP32**, et d'envoyer ces données en temps réel vers un tableau de bord **ThingsBoard/ThingSpeak** via le protocole MQTT.

## Fonctionnalités

* **Lecture de la Télé-information (TIC) :** Décodage des trames du Linky en mode Historique (1200 bauds, 7E1).
* **Extraction des données clés :** * `PAPP` : Puissance Apparente (VA) - Consommation en temps réel.
  * `BASE` : Index de consommation total (Wh).
  * `IINST` : Intensité Instantanée (A).
  * `ISOUSC` : Intensité Souscrite (A).
* **Connectivité Wi-Fi :** Connexion automatique au réseau local.
* **Télémétrie MQTT :** Envoi des données formatées en JSON vers le cloud ThingsBoard.

## Matériel Requis

* 1x Carte **ESP32** (ex: ESP32 Dev Module / NodeMCU).
* 1x **Module Télé-info (TIC)** (module tout fait ou montage maison avec optocoupleur et résistances) pour adapter le signal du Linky (qui est en alternatif) vers un signal lisible par l'ESP32.
* Fils de connexion (Câble téléphonique ou RJ45 pour relier le Linky au module).

## Câblage

Le compteur Linky dispose de 3 bornes (I1, I2, A). Vous devez vous brancher sur **I1 et I2** (pas de polarité).

| Module TIC | ESP32 |
| :--- | :--- |
| VCC / 3.3V | 3.3V |
| GND | GND |
| TX / OUT | **GPIO 16** (RX2) |

*(Note : Le port série matériel `Serial2` de l'ESP32 est utilisé dans ce code pour lire la broche 16).*

## Prérequis Logiciels

Ce projet a été développé avec l'**IDE Arduino**. Vous devez installer les bibliothèques suivantes via le gestionnaire de bibliothèques :

1. **LibTeleinfo** (par *Charles Hallard*) - Pour le décodage des trames complexes du Linky.
2. **PubSubClient** (par *Nick O'Leary*) - Pour la communication MQTT avec ThingsBoard.

Assurez-vous également d'avoir ajouté le support des cartes ESP32 d'Espressif dans votre IDE Arduino.

## ⚙️ Configuration

Avant de téléverser le code, ouvrez le fichier `.ino` et modifiez les variables suivantes avec vos propres informations :

```cpp
// --- CONFIGURATION WIFI ---
const char* ssid = "VOTRE_NOM_DE_WIFI";
const char* password = "VOTRE_MOT_DE_PASSE_WIFI";

// --- CONFIGURATION THINGSBOARD ---
const char* mqtt_server = "thingsboard.cloud"; // Laissez tel quel pour le cloud public
const char* token = "VOTRE_ACCESS_TOKEN_THINGSBOARD";
```

Pour obtenir votre token, créez un compte sur ThingsBoard, ajoutez un nouvel "Appareil" (Device) et copiez son Jeton d'accès (Access Token).

##  Installation et Déploiement

* Connectez l'ESP32 à votre ordinateur via USB.
* Sélectionnez ESP32 Dev Module dans Outils > Type de carte.
* Cliquez sur Téléverser. (Si la connexion stagne sur Connecting..., maintenez le bouton BOOT de l'ESP32 enfoncé quelques secondes).
* Ouvrez le Moniteur Série (réglé sur 115200 bauds) pour vérifier que l'ESP32 se connecte bien au Wi-Fi et commence à envoyer les données.
* Rendez-vous sur votre interface ThingsBoard, dans les détails de votre appareil, onglet Latest Telemetry, pour voir vos données arriver en direct !

📜 Licence
Ce projet est libre de droits. N'hésitez pas à l'adapter et à l'améliorer pour vos propres tableaux de bord !
