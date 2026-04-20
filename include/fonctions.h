#ifndef FONCTIONS_H
#define FONCTIONS_H
#include <WiFi.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ICM20948.h>

extern Adafruit_ICM20948 icm;
extern WiFiServer server;


void moteur(int pwm1 , int pwm2);
void monte(int taille_esc, int vitesse, sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag);
void tourne(sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag);
void info_etat(WiFiClient &client, sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag);
void nb_avance (int &nb_tr_gauche, int &nb_tr_avance, int &etat, WiFiClient client, sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag);
void afficher_icm(WiFiClient &client, sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag);

#endif