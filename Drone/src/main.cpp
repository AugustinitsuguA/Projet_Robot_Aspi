// Adresse MAC = 00:70:07:2D:24:9C
// Adresse MAC robot escalier = 1C:DB:D4:78:50:04

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "../include/fonctions.h"
#include <cmath>
#include <ESP32Servo.h>

#pragma region init variables
    // Enum de la machine à état
    enum State {
        OFF,
        INIT,
        GO_TO_STEP,
        GO_TO_PLATFORM,
        CLEANING,
        SEARCHING,
        WAITING
    };

    // Information sur l'ESP avec qui on communique
    esp_now_peer_info_t peerInfo;

    // Affectation des broches capteur ultrason
    #define TRIG1 32
    #define ECHO1 33
    #define TRIG2 26
    #define ECHO2 25

    // Affectation broche IR
    #define IR 13 

    // Affectation broche Pression
    #define PRES 35

    // Affectation broche Moteurs
    #define MOT_G 22
    #define MOT_D 23
    Servo mot_g;
    Servo mot_d;

    // Affecation broche ventilateur
    #define VENT_PWM 34

    // Variable de machine à état
    State drone_state = INIT;
#pragma endregion


void setup() {
    Serial.begin(115200);

#pragma region init comm wifi
    // Configuration en mode station pour l'ESP-NOW sans se connecter à un réseau
    WiFi.mode(WIFI_STA);

    if (esp_now_init() != ESP_OK) {
        Serial.println("Erreur init ESP-NOW");
        return;
    }
    // Initialise "OnDataRecv" et "OnDataSent" comme fonction d'appel à chaque réception et envoie respectivement
    esp_now_register_recv_cb(OnDataRecv);
    esp_now_register_send_cb(OnDataSent);

    // Ajout du pair distant (nécessaire pour pouvoir lui envoyer des données)
    memcpy(peerInfo.peer_addr, peerMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    // Enregistrement du pair sur la comm ESP NOW
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Échec ajout du pair");
        return;
    }
#pragma endregion

#pragma region init pin capteurs
    // Ultrason Gauche
    pinMode(TRIG1, OUTPUT);
    pinMode(ECHO1, INPUT);

    // Ultrason Droite
    pinMode(TRIG2, OUTPUT);
    pinMode(ECHO2, INPUT);

    // IR
    pinMode(IR, INPUT);

    // Pression
    pinMode(PRES, INPUT);
#pragma endregion

#pragma region init pin moteurs
    // Moteur Gauche
    pinMode(MOT_G, OUTPUT);
    mot_g.attach(MOT_G);

    // Moteur Droite
    pinMode(MOT_D, OUTPUT);
    mot_d.attach(MOT_D);

    // Aspirateur
    pinMode(VENT_PWM, OUTPUT);
    pinMode(VENT_EN, OUTPUT);

#pragma endregion

}

void loop() {

#pragma region machine à état
    if (INIT == drone_state) {

    }

    if (WAITING == drone_state){
        // if ('marche atteinte' == message reçu) {
        //      drone_state = GO_TO_STEP}  
    }

    if (GO_TO_STEP == drone_state){
        // Activer moteurs
        // if (dist_ir < 5){
        //      Mot_d = 100
        //      Mot_g = -100
        //      Voir temps nécessaire pour faire un quart de tour}
        // drone_state = CLEANING 
        // Désactiver moteurs     
    }

    if (CLEANING == drone_state){
        // i = 0
        // Activer moteurs
        // Activer ventilateur
        // if (dist_ir < 5){
        //      Mot_d = 100
        //      Mot_g = -100
        //      Voir temps nécessaire pour faire un demi de tour}
        // i = 1 
        // if (dist_ir < 5){
        //      Mot_d = 100
        //      Mot_g = -100
        //      Voir temps nécessaire pour faire un demi de tour}
        // drone_state = SEARCHING
        // Désactiver ventilateur
    }

    if (SEARCHING == drone_state){
        // Activer moteurs
        // if ('robot_detecte' = message_reçu){
        //      drone_state = GO_TO_PLATFORM
        //      Désactiver moteurs}
    }

    if (GO_TO_PLATFORM == drone_state){
        // Activer moteurs
        // if (dist_ir < 3){
        //      Désactiver moteurs
        //      ENVOYER message 'plateforme atteinte'
        //      drone_state = WAITING}
    }
#pragma endregion

#pragma region test wifi
    donneesEnvoyees.message = "Test";
    // Envoie des données sur l'adresse MAC en convertissant les données en pointeur de données bruts
    //esp_now_send(peerMAC, (uint8_t *) &donneesEnvoyees, sizeof(donneesEnvoyees));

    // Test comm
    //Serial.print("Message : ");
    //Serial.print(donneesRecues.message);
    //Serial.println();
#pragma endregion

#pragma region test ultrason
    // Test capteur ultrason
    us_mes(TRIG1, ECHO1);
    us_mes(TRIG2, ECHO2);
    delay(1000);
#pragma endregion

#pragma region test capteur ir
    // Test capteur IR
    //ir_mes(IR);
    //delay(500);
#pragma endregion

#pragma region test moteurs
    // Test moteurs
    //mot_g.writeMicroseconds(1000);
    //mot_d.writeMicroseconds(1000);
    //delay(1000);
    //mot_g.writeMicroseconds(1500);
    //mot_d.writeMicroseconds(1500);
    //delay(1000);
    //mot_g.writeMicroseconds(2000);
    //mot_d.writeMicroseconds(2000);
#pragma endregion
}
