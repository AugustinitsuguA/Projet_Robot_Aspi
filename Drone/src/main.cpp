#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "../include/fonctions.h"


// Capteur ultrason
#define TRIG_PIN 32
#define ECHO_PIN 33

void setup() {
    Serial.begin(115200);


    // Test communication esp NOW
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Erreur init ESP-NOW");
        return;
    }
    esp_now_register_recv_cb(OnDataRecv);

    
    // Initialisation capteur ultrason
    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

void loop() {





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

    Serial.print("Distance : ");
    Serial.print(distance);
    Serial.println(" cm");
    //delay(200);
}
