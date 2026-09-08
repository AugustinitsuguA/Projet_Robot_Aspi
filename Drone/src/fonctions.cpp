#include "../include/fonctions.h"
#include <WiFi.h>
#include <ESP32Servo.h>

void avancer(servo &servo_gauche, servo &servo_droit, int vitesse) {
  servo_gauche.write(vitesse);
  servo_droit.write(vitesse);
}

void reculer(servo &servo_gauche, servo &servo_droit, int vitesse) {
  servo_gauche.write(-vitesse);
  servo_droit.write(-vitesse);
}

void gauche (servo &servo_gauche, servo &servo_droit, int vitesse) {
  servo_gauche.write(-vitesse);
  servo_droit.write(vitesse);
}

void droite (servo &servo_gauche, servo &servo_droit, int vitesse) {
  servo_gauche.write(vitesse);
  servo_droit.write(-vitesse);
}
