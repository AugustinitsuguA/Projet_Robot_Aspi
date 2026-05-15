#include <MadgwickAHRS.h>
#include <Adafruit_ICM20X.h>
#include <Adafruit_ICM20948.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include "../include/fonctions.h"


Madgwick filter;
Servo servo_platforme;


const char* ssid = "ESP32_ROBOT";
const char* password = "12345678";

WiFiServer server(1235);   // serveur TCP
WiFiServer server_icm(1234);

#define SCL 6
#define SDA 5

// pin servo plateforme
//#define PLATFORME 0

Adafruit_ICM20948 icm;
uint16_t measurement_delay_us = 65535; // Delay between measurements for testing
// For SPI mode, we need a CS pin
#define ICM_CS 10
// For software-SPI mode we need SCK/MOSI/MISO pins
#define ICM_SCK 13
#define ICM_MISO 12
#define ICM_MOSI 11
// pins du driver
#define MOTOR_D_IN1 10
#define MOTOR_D_IN2 11
#define MOTOR_G_IN1 12
#define MOTOR_G_IN2 13


/*
différents états du robot : 


*/

void setup() {
  Serial.begin(115200);
  Serial.println("demarrage du robot");
  delay(1000);
 
  Serial.println("robot test");

  // pins du driver
  pinMode(MOTOR_D_IN1, OUTPUT);
  pinMode(MOTOR_D_IN2, OUTPUT);
  pinMode(MOTOR_G_IN1, OUTPUT);
  pinMode(MOTOR_G_IN2, OUTPUT);

  servo_platforme.attach(37);  // Attache le servo à la broche 0
  servo_platforme.write(90); // Positionne le servo à 90 degrés (position neutre)

  Wire.begin(SDA, SCL);  // Definition pin d'information

  filter.begin(20);

  // Try to initialize!
  if (!icm.begin_I2C()) {
    // if (!icm.begin_SPI(ICM_CS)) {
    // if (!icm.begin_SPI(ICM_CS, ICM_SCK, ICM_MISO, ICM_MOSI)) {

    Serial.println("Failed to find ICM20948 chip");
    while (1) {
      delay(10);
    }
  }
  
    // Connexion WiFi (mode point d'accès)
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  WiFi.setSleep(false);

  Serial.println("\nPoint d'acces WiFi lance !");
  Serial.print("Adresse IP : ");
  Serial.println(WiFi.softAPIP());

  server.begin();
  server_icm.begin();
  Serial.println("Serveur TCP lance");



  Serial.println("ICM20948 Found!");
  // icm.setAccelRange(ICM20948_ACCEL_RANGE_16_G);
  Serial.print("Accelerometer range set to: ");
  switch (icm.getAccelRange()) {
  case ICM20948_ACCEL_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case ICM20948_ACCEL_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case ICM20948_ACCEL_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case ICM20948_ACCEL_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  Serial.println("OK");

  // icm.setGyroRange(ICM20948_GYRO_RANGE_2000_DPS);
  Serial.print("Gyro range set to: ");
  switch (icm.getGyroRange()) {
  case ICM20948_GYRO_RANGE_250_DPS:
    Serial.println("250 degrees/s");
    break;
  case ICM20948_GYRO_RANGE_500_DPS:
    Serial.println("500 degrees/s");
    break;
  case ICM20948_GYRO_RANGE_1000_DPS:
    Serial.println("1000 degrees/s");
    break;
  case ICM20948_GYRO_RANGE_2000_DPS:
    Serial.println("2000 degrees/s");
    break;
  }

  //  icm.setAccelRateDivisor(4095);
  uint16_t accel_divisor = icm.getAccelRateDivisor();
  float accel_rate = 1125 / (1.0 + accel_divisor);

  Serial.print("Accelerometer data rate divisor set to: ");
  Serial.println(accel_divisor);
  Serial.print("Accelerometer data rate (Hz) is approximately: ");
  Serial.println(accel_rate);

  //  icm.setGyroRateDivisor(255);
  uint8_t gyro_divisor = icm.getGyroRateDivisor();
  float gyro_rate = 1100 / (1.0 + gyro_divisor);

  Serial.print("Gyro data rate divisor set to: ");
  Serial.println(gyro_divisor);
  Serial.print("Gyro data rate (Hz) is approximately: ");
  Serial.println(gyro_rate);

  // icm.setMagDataRate(AK09916_MAG_DATARATE_10_HZ);
  Serial.print("Magnetometer data rate set to: ");
  switch (icm.getMagDataRate()) {
  case AK09916_MAG_DATARATE_SHUTDOWN:
    Serial.println("Shutdown");
    break;
  case AK09916_MAG_DATARATE_SINGLE:
    Serial.println("Single/One shot");
    break;
  case AK09916_MAG_DATARATE_10_HZ:
    Serial.println("10 Hz");
    break;
  case AK09916_MAG_DATARATE_20_HZ:
    Serial.println("20 Hz");
    break;
  case AK09916_MAG_DATARATE_50_HZ:
    Serial.println("50 Hz");
    break;
  case AK09916_MAG_DATARATE_100_HZ:
    Serial.println("100 Hz");
    break;
  }
  Serial.println();
}

void loop() {
    //  /* Get a new normalized sensor event */
  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t mag;
  sensors_event_t temp;
 
  Serial.println("debut loop");
  
  //int nb_tr_droite;
  float accel_x;
  float accel_y;
  float accel_z;
  float mag_x;
  float mag_y;
  float mag_z;
  float gyro_x;
  float gyro_y;
  float gyro_z;
  int y_up;
  int y_up_p;
  int nb_tr_avance = 0;
  int nb_tr_gauche = 0;
  int nb_tr_droite = 0;
  int nb_marche = 0;

  while(1){

    // l'interface py envoit des valeurs à l'esp32 via WIFI
    // en fonction du premier mot reçu, l'esp effectue une action ou donne des infos au gui
    WiFiClient client = server.available();

    if (client) {
      Serial.println("Client connecte");

      while (client.connected()) {
        if (client.available()) {

          String msg = client.readStringUntil('\n');
          msg.trim(); 
          Serial.print("Recu : ");
          Serial.println(msg);


          // juste pour tester un bouton du gui et que l'icm marche
          if (msg == "info_etat") {
            info_etat(client, accel, gyro, temp, mag);
          }

          // réinitialiser les valeurs de comptage 
          if (msg == "init_valeurs") {
            Serial.println("valeurs reinitialisées");
            nb_tr_avance = 0;
            nb_tr_gauche = 0;
            nb_tr_droite = 0;
  
          }

          // monte les escaliers
          //118
          if (msg.substring(0,14) == "monte_escalier") { //modif
            int pos1 = msg.indexOf(";");
            //Serial.println(pos1);
            int pos2 = msg.indexOf(";",pos1+1);
            int pos3 = msg.indexOf(";",pos2+1);
            String chaine1 = msg.substring(0,pos1);
            String chaine2 = msg.substring(pos1+1,pos2);
            int taille_esc = msg.substring(pos2+1,pos3).toInt();
            monte(client, taille_esc, chaine2.toInt(), accel , gyro , temp , mag);
          }


          if (msg.substring(0,6) == "tourne") { 
            int pos1 = msg.indexOf(";");
            //Serial.println(pos1);
            int pos2 = msg.indexOf(";",pos1+1);
            
            int chaine1 = msg.substring(pos1+1,pos2).toInt();
            tourne(client, chaine1, accel , gyro , temp , mag);
          }

          if (msg.substring(0,16) == "platforme_monter") {
            int pos1 = msg.indexOf(";"); 
            int pos2 = msg.indexOf(";",pos1+1);
            int angle = msg.substring(pos1+1,pos2).toInt();
            int duree = msg.substring(pos2+1,-1).toInt();
            Serial.println("Monter la plateforme");
            Serial.print(angle);Serial.println("°");
            plateforme(client, servo_platforme, angle, duree);
            //Serial.print(angle);
            //servo_platforme.write(angle); // Positionne le servo à 180 degrés pour monter la plateforme
          } 

          if (msg.substring(0,16) == "avance_controlee") {
            int pos1 = msg.indexOf(";"); 
            int pos2 = msg.indexOf(";",pos1+1);
            int pos3 = msg.indexOf(";",pos2+1);
            int pos4 = msg.indexOf(";",pos3+1);
            int pos5 = msg.indexOf(";",pos4+1);
            int vitesse1 = msg.substring(pos1+1,pos2).toInt();
            int vitesse2 = msg.substring(pos2+1,pos3).toInt();
            int vitesse1D = msg.substring(pos3+1,pos4).toInt();
            int vitesse2D = msg.substring(pos4+1,pos5).toInt();
            int temps = msg.substring(pos5+1,-1).toInt();
            Serial.println("Avance contrôlée");
            avance_controlee(client, vitesse1, vitesse2, vitesse1D, vitesse2D, temps, accel , gyro , temp , mag);
  
          } 

           

          // ------- afficher les données de l'ICM dans le but de les enregistrer en txt
          // ------- copier l'output et le coller dans un txt
          if (msg == "afficher_icm") {
            afficher_icm(client, accel , gyro , temp , mag);
          }

          

        // sert à envoyer les données pour controler les moteurs
        if (msg.substring(0,2) == "m1") { // découpe la chaine de caractere envoyée par le gui de python
              // message sous la forme m1:64;m2:82;
            int pos1 = msg.indexOf(";");
            //Serial.println(pos1);
            int pos2 = msg.indexOf(";",pos1+1);
            String chaine1 = msg.substring(0,pos1);
            String chaine2 = msg.substring(pos1+1,pos2);
            //Serial.println(pos2);
            //Serial.println(chaine1);
            //Serial.println(chaine2);

            int pos11 = chaine1.indexOf(":");
            String chaine11 = chaine1.substring(0,pos11);
            String chaine12 = chaine1.substring(pos11+1,-1);
            
            //Serial.println(chaine11);
            //Serial.println(chaine12);

            int pos22 = chaine2.indexOf(":");
            String chaine21 = chaine2.substring(0,pos22);
            String chaine22 = chaine2.substring(pos22+1,-1);
            
            Serial.print(chaine11);
            Serial.print(" a pour valeur : ");
            Serial.print(chaine12);
            Serial.print(" /// ");
            Serial.print(chaine21);
            Serial.print(" a pour valeur : ");
            Serial.println(chaine22);

            moteur(chaine12.toInt(), chaine22.toInt()); // fait tourner les moteurs

          }
          /*
          // message sous la forme m1:64;m2:82;
            int pos1 = msg.indexOf(";");
            //Serial.println(pos1);
            int pos2 = msg.indexOf(";",pos1+1);
            String chaine1 = msg.substring(0,pos1);
            String chaine2 = msg.substring(pos1+1,pos2);
            int pos11 = chaine1.indexOf(":");
            String chaine11 = chaine1.substring(0,pos11);
            String chaine12 = chaine1.substring(pos11+1,-1);
            int pos22 = chaine2.indexOf(":");
            String chaine21 = chaine2.substring(0,pos22);
            String chaine22 = chaine2.substring(pos22+1,-1);
            
            Serial.print(chaine11);
            Serial.print(" a pour valeur : ");
            Serial.print(chaine12);
            Serial.print(" /// ");
            Serial.print(chaine21);
            Serial.print(" a pour valeur : ");
            Serial.println(chaine22);

            moteur(chaine12.toInt(), chaine22.toInt()); // fait tourner les moteurs
            */
            }
        delay(10);
      }

      client.stop();
      Serial.println("Client deconnecte");
    }


    delay(5);

    }
}
