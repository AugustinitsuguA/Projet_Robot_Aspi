// Adresse Mac = 00:70:07:2D:24:9C
// Adresse MAC robot escalier = 1C:DB:D4:78:50:04

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "../include/fonctions.h"

// Test comm wifi
esp_now_peer_info_t peerInfo;


// Capteur ultrason
#define TRIG_PIN 32
#define ECHO_PIN 33

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Erreur init ESP-NOW");
        return;
    }
    // Callback
    esp_now_register_recv_cb(OnDataRecv);
    esp_now_register_send_cb(OnDataSent);

    // Ajout du pair distant (nécessaire pour pouvoir lui envoyer des données)
    memcpy(peerInfo.peer_addr, peerMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Échec ajout du pair");
        return;
    }


    // Initialisation capteur ultrason
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

void loop() {
    donneesEnvoyees.capteur = 1;
    donneesEnvoyees.valeur = 2;
    esp_now_send(peerMAC, (uint8_t *) &donneesEnvoyees, sizeof(donneesEnvoyees));

    // Test comm wifi
    Serial.print("Dernière valeur reçue capteur ");
    Serial.print(donneesRecues.capteur);
    Serial.print(" : ");
    Serial.println(donneesRecues.valeur);




    // Test capteur ultrason
    // On s'assure que TRIG est à LOW
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    // Impulsion de 10 µs
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Mesure de la durée de l'écho
    long duration = pulseIn(ECHO_PIN, HIGH);

    // Calcul de la distance en cm
    float distance = duration * 0.0343 / 2;

    //Serial.print("Distance : ");
    //Serial.print(distance);
    //Serial.println(" cm");
    //delay(200);
}
