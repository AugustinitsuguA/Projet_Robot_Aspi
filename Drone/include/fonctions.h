#ifndef FONCTIONS_H
#define FONCTIONS_H

#include <WiFi.h>
#include <ESP32Servo.h>
#include <esp_now.h>


// Structure partagée entre tous les fichiers
typedef struct struct_message {
  int capteur;
  float valeur;
} struct_message;

// Déclaration de la variable (définie ailleurs, ici on annonce juste qu'elle existe)
extern struct_message donneesRecues;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);

#endif