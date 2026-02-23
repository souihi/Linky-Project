#include <WiFi.h>
#include <LibTeleinfo.h>
#include "ThingSpeak.h"

// --- CONFIGURATION WIFI ---
const char* ssid = "VOTRE_WIFI";
const char* password = "VOTRE_MOT_DE_PASSE";

// --- CONFIGURATION THINGSPEAK ---
unsigned long myChannelNumber = 123456;         // Remplacez par votre Channel ID
const char * myWriteAPIKey = "VOTRE_API_KEY";   // Remplacez par votre clé Write API

// --- CONFIGURATION MATERIELLE ---
#define RX_PIN 16  // Broche RX connectée au Linky

TInfo tinfo;
WiFiClient client;
unsigned long last_sent = 0;

// Variables pour stocker les valeurs
long p_app = 0;
long index_base = 0;
long i_inst = 0;
long i_sousc = 0;

// Fonction appelée automatiquement par LibTeleinfo à chaque nouvelle donnée
void DataCallback(ValueList * me, uint8_t flags) {
  if (strcmp(me->name, "PAPP") == 0) p_app = atol(me->value);
  if (strcmp(me->name, "BASE") == 0) index_base = atol(me->value);
  if (strcmp(me->name, "IINST") == 0) i_inst = atol(me->value);
  if (strcmp(me->name, "ISOUSC") == 0) i_sousc = atol(me->value);
}

void setup() {
  Serial.begin(115200); // Pour voir les messages sur le PC
  
  // Initialisation du port série Linky : 1200 bauds, 7 bits, Parité Paire
  Serial2.begin(1200, SERIAL_7E1, RX_PIN, -1); 
  
  tinfo.init();
  tinfo.attachData(DataCallback); 

  // Connexion WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connexion WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connecté !");
  
  ThingSpeak.begin(client);
}

void loop() {
  // 1. Lire les données du Linky en continu
  if (Serial2.available()) {
    tinfo.process(Serial2.read()); 
  }

  // 2. Envoyer à ThingSpeak toutes les 30 secondes
  if (millis() - last_sent > 30000) {
    if(WiFi.status() == WL_CONNECTED){
        ThingSpeak.setField(1, (long)p_app);
        ThingSpeak.setField(2, (long)index_base);
        ThingSpeak.setField(3, (long)i_inst);
        ThingSpeak.setField(4, (long)i_sousc);
        
        int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
        if(x == 200) {
          Serial.println("Mise à jour ThingSpeak réussie.");
        } else {
          Serial.println("Erreur ThingSpeak. Code HTTP: " + String(x));
        }
        last_sent = millis();
    }
  }
}