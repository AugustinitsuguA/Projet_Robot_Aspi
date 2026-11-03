// Adresse MAC = 00:70:07:2D:24:9C
// Adresse MAC robot escalier = 1C:DB:D4:78:50:04

#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include "../include/fonctions.h"
#include <cmath>

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
    #define TRIG_G 32
    #define ECHO_G 33
    #define TRIG_D 26
    #define ECHO_D 25

    // Affectation broche IR
    #define IR 35 

    // Affectation broche Pression
    #define PRES 34

    // Affectation broche Moteurs
    #define MOT_G 22
    #define MOT_D 23

    // Affecation broche ventilateur
    #define VENT 13

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
    pinMode(TRIG_G, OUTPUT);
    pinMode(ECHO_G, INPUT);

    // Ultrason Droite
    pinMode(TRIG_D, OUTPUT);
    pinMode(ECHO_D, INPUT);

    // IR
    pinMode(IR, INPUT);

    // Pression
    pinMode(PRES, INPUT);

    // Moteur Gauche
    pinMode(MOT_G, OUTPUT);

    // Moteur Droite
    pinMode(MOT_D, OUTPUT);

    // Aspirateur
    pinMode(VENT, OUTPUT);
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
    donneesEnvoyees.capteur = 1;
    donneesEnvoyees.valeur = 2;
    // Envoie des données sur l'adresse MAC en convertissant les données en pointeur de données bruts
    esp_now_send(peerMAC, (uint8_t *) &donneesEnvoyees, sizeof(donneesEnvoyees));

    // Test comm wifi
    Serial.print("Dernière valeur reçue capteur ");
    Serial.print(donneesRecues.capteur);
    Serial.print(" : ");
    Serial.println(donneesRecues.valeur);
#pragma endregion


#pragma region test ultrason
    // Test capteur ultrason
    // On s'assure que TRIG est à LOW
    digitalWrite(TRIG_G, LOW);
    delayMicroseconds(2);

    // Impulsion de 10 µs
    digitalWrite(TRIG_G, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_G, LOW);

    // Mesure de la durée de l'écho
    long duration = pulseIn(ECHO_G, HIGH);

    // Calcul de la distance en cm
    float distance = duration * 0.0343 / 2;

    //Serial.print("Distance : ");
    //Serial.print(distance);
    //Serial.println(" cm");
    //delay(100);
#pragma endregion

#pragma region test capteur ir
    float volt_ir = analogRead(IR)*3.3/4092;
    float dist_ir = 3.8868 * std::pow(volt_ir,4) - 31.349 * std::pow(volt_ir,3) + 91.88 * std::pow(volt_ir,2) - 119.89 * volt_ir + 67.636;
    //float dist_ir = (volt_ir-3.3)/(-0.140);
    //Serial.print("Distance : ");
    //Serial.print(dist_ir);
    //Serial.print(" cm / Tension : ");
    //Serial.println(volt_ir);
#pragma endregion
}
