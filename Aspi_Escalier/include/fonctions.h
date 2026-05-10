#ifndef FONCTIONS_H
#define FONCTIONS_H
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ICM20948.h>
#include <ESP32Servo.h>

extern Adafruit_ICM20948 icm;
extern WiFiServer server;
extern Servo servo_platforme;


void moteur(int pwm1 , int pwm2);
void monte(WiFiClient &client, int taille_esc, int vitesse, sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag);
void tourne(WiFiClient &client, int nb_tours, sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag);
void info_etat(WiFiClient &client, sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag);
void nb_avance (int &nb_tr_gauche, int &nb_tr_avance, int &etat, WiFiClient client, sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag);
void afficher_icm(WiFiClient &client, sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag);
void avance_controlee(WiFiClient &client, int vitesse_1, int vitesse_2, int vitesse_1D, int vitesse_2D, int temps_1, sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag);
int arret(WiFiClient &client);
void envoie_donnees(WiFiClient &client);
void plateforme(WiFiClient &client, Servo &servo_platforme, int vitesse, int temps);

#endif