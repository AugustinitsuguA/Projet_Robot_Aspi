#ifndef FONCTIONS_H
#define FONCTIONS_H

#include <WiFi.h>
#include <ESP32Servo.h>
#include <esp_now.h>


// Structure partagée entre tous les fichiers
typedef struct struct_message {
  String message;  
} struct_message;

// Déclaration de la variable (définie ailleurs, ici on annonce juste qu'elle existe)
extern struct_message donneesRecues;
extern struct_message donneesEnvoyees;
extern uint8_t peerMAC[6];
extern long duration;
extern float distance;
extern float volt_ir;
extern float dist_ir;

void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void us_mes(int TRIG, int ECHO);
void ir_mes(int IR);

#endif