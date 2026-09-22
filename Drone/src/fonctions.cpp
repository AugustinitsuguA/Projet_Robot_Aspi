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
  Serial.print("Capteur: ");
  Serial.print(donneesRecues.capteur);
  Serial.print(" | Valeur: ");
  Serial.println(donneesRecues.valeur);
}

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Statut envoi: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Succès" : "Échec");
}