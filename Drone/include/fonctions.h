#ifndef FONCTIONS_H
#define FONCTIONS_H
#include <WiFi.h>
#include <ESP32Servo.h>

void avancer( servo &servo_gauche, servo &servo_droit, int vitesse);
void reculer( servo &servo_gauche, servo &servo_droit, int vitesse);
void gauche ( servo &servo_gauche, servo &servo_droit, int vitesse);
void droite ( servo &servo_gauche, servo &servo_droit, int vitesse);
