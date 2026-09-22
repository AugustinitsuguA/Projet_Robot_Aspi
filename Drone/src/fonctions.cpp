#include "../include/fonctions.h"
#include <WiFi.h>
#include <ESP32Servo.h>
#include <esp_now.h>

struct_message donneesRecues;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&donneesRecues, incomingData, sizeof(donneesRecues));
  Serial.print("Capteur: ");
  Serial.print(donneesRecues.capteur);
  Serial.print(" | Valeur: ");
  Serial.println(donneesRecues.valeur);
}