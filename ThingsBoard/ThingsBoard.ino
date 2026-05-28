#include <WiFi.h>
#include <LibTeleinfo.h>
#include <PubSubClient.h>

// --- CONFIGURATION WIFI ---
const char* ssid = "VOTRE_WIFI";
const char* password = "VOTRE_MOT_DE_PASSE";

// --- CONFIGURATION THINGSBOARD ---
const char* mqtt_server = "eu.thingsboard.cloud"; // Laissez tel quel si vous utilisez le cloud officiel européen
const char* token = "VOTRE_ACCESS_TOKEN";      // Le token généré par ThingsBoard pour votre appareil

// --- CONFIGURATION MATERIELLE ---
#define RX_PIN 16

TInfo tinfo;
WiFiClient espClient;
PubSubClient client(espClient);
unsigned long last_sent = 0;

// Variables
long p_app = 0;
long index_base = 0;
long i_inst = 0;
long i_sousc = 0;

// Callback Teleinfo
void DataCallback(ValueList * me, uint8_t flags) {
  if (strcmp(me->name, "PAPP") == 0) p_app = atol(me->value);
  if (strcmp(me->name, "BASE") == 0) index_base = atol(me->value);
  if (strcmp(me->name, "IINST") == 0) i_inst = atol(me->value);
  if (strcmp(me->name, "ISOUSC") == 0) i_sousc = atol(me->value);
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Connexion à ThingsBoard...");
    // Connexion avec le Token comme nom d'utilisateur
    if (client.connect("ESP32_Linky", token, NULL)) {
      Serial.println("Connecté !");
    } else {
      Serial.print("Échec, rc=");
      Serial.print(client.state());
      Serial.println(" Nouvelle tentative dans 5 secondes");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(1200, SERIAL_7E1, RX_PIN, -1); 
  
  tinfo.init();
  tinfo.attachData(DataCallback); 

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // Maintient la connexion MQTT active

  // 1. Lire le Linky
  if (Serial2.available()) {
    tinfo.process(Serial2.read()); 
  }

  // 2. Envoyer la télémétrie toutes les 10 secondes (MQTT est rapide, on peut l'envoyer plus souvent)
  if (millis() - last_sent > 10000) {
    // Construction du format JSON : {"PAPP":190, "BASE":5662710...}
    String payload = "{";
    payload += "\"PAPP\":" + String(p_app) + ",";
    payload += "\"BASE\":" + String(index_base) + ",";
    payload += "\"IINST\":" + String(i_inst) + ",";
    payload += "\"ISOUSC\":" + String(i_sousc);
    payload += "}";

    // Envoi sur le topic spécifique de ThingsBoard
    client.publish("v1/devices/me/telemetry", (char*) payload.c_str());
    
    Serial.println("Données envoyées à ThingsBoard : " + payload);
    last_sent = millis();
  }
}
