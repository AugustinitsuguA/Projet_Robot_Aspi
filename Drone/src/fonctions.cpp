// Adresse MAC robot escalier = 1C:DB:D4:78:50:04

#include "../include/fonctions.h"
#include <WiFi.h>
#include <ESP32Servo.h>
#include <esp_now.h>

struct_message donneesRecues;
struct_message donneesEnvoyees;

uint8_t peerMAC[6] = {0x1C, 0xDB, 0xD4, 0x78, 0x50, 0x04};

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&donneesRecues, incomingData, sizeof(donneesRecues));
  Serial.print("Message: ");
  Serial.print(donneesRecues.message);
  Serial.println();
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Statut envoi: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Succès" : "Échec");
}

long duration;
float distance;

void us_mes(int TRIG, int ECHO){
  // On s'assure que TRIG est à LOW
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);

    // Impulsion de 10 µs
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    // Mesure de la durée de l'écho
    duration = pulseIn(ECHO, HIGH);

    // Calcul de la distance en cm
    distance = duration * 0.0343 / 2;

    Serial.print("Distance capteur US pin ");
    Serial.print(TRIG);
    Serial.print(" : ");
    Serial.print(distance);
    Serial.println(" cm");
}

float volt_ir;
float dist_ir;

void ir_mes(int IR){
  volt_ir = analogRead(IR)*3.3/4092;
  dist_ir = 3.8868 * std::pow(volt_ir,4) - 31.349 * std::pow(volt_ir,3) + 91.88 * std::pow(volt_ir,2) - 119.89 * volt_ir + 67.636;
  Serial.print("Distance capteur IR pin ");
  Serial.print(IR);
  Serial.print(" : ");
  Serial.print(dist_ir);
  Serial.println(" cm");
}