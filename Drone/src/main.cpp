#include <Arduino.h>
#include <WiFi.h>


#define MOTEUR_GAUCHE 3
#define MOTEUR_DROIT 4

const char* ssid = "ESP32_ROBOT";
const char* password = "12345678";

const char* host = "192.168.4.1";
const uint16_t port = 1234;

WiFiClient client;

void setup() {
  Serial.begin(115200);

  WiFi.begin("NomWifi", "MotDePasse");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.localIP());
  
  /*
  Serial.begin(115200);
  Serial.println("demarrage du drone");
  pinMode(MOTEUR_GAUCHE, OUTPUT);
  pinMode(MOTEUR_DROIT,OUTPUT);

  // Connexion au point d'accès du principal
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Connexion");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Connecté au WiFi");
  Serial.print("IP locale : ");
  Serial.println(WiFi.localIP());

  // Connexion TCP au serveur du principal
  if (client.connect(host, port)) {
    Serial.println("Connecté au serveur TCP");
  } else {
    Serial.println("Connexion TCP échouée");
  }
  */
}

void loop() {

  if (client) {
      Serial.println("Client connecte");

      while (client.connected()) {
        if (client.available()) {

          String msg = client.readStringUntil('\n');
          msg.trim(); 
          Serial.print("Recu : ");
          Serial.println(msg);


          // juste pour tester un bouton du gui et que l'icm marche
          if (msg == "test") {
            Serial.println("ok test");
          }
        }

      }
    }
    
  delay(50);
}
