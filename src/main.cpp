#include <MadgwickAHRS.h>
#include <Adafruit_ICM20X.h>
#include <Adafruit_ICM20948.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <WiFi.h>
#include "../include/fonctions.h"


Madgwick filter;

const char* ssid = "ESP32_ROBOT";
const char* password = "12345678";

WiFiServer server(1234);   // serveur TCP

#define SCL 6
#define SDA 5

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

-1 --> état de base : permet de communiquer avec le GUI, faire les tests des différents actionneurs, capteurs
          - on lance l'affichage des données de l'IMU dans un format qui permet d'être copié dans un txt pour afficher sur MATLAB
          - controle des moteurs
          - remettre les variables à zéro
          (- mouvement de la plateforme)

0 --> état test en vue du développement de l'état 1
          - affiche les données de position
          (- controle des moteurs)
      
1 --> état autonome, qui sera l'état final  du système
          - affiche les données de position
          - mouvement, arrêts, déploiement de robot aspi : autonome
          

*/

void setup() {
    Serial.begin(115200);
  
  delay(1000);
 
  Serial.println("robot test");

  // pins du driver
  pinMode(MOTOR_D_IN1, OUTPUT);
  pinMode(MOTOR_D_IN2, OUTPUT);
  pinMode(MOTOR_G_IN1, OUTPUT);
  pinMode(MOTOR_G_IN2, OUTPUT);


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
  int etat = 1; 
  int taille_esc = 14;

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
            icm.getEvent(&accel, &gyro, &temp, &mag);
            accel_x = accel.acceleration.x ;
            accel_y = accel.acceleration.y ;
            accel_z = accel.acceleration.z ;
            mag_x = mag.magnetic.x ;
            mag_y = mag.magnetic.y ;
            mag_z = mag.magnetic.z ;
            gyro_x = gyro.gyro.x ;
            gyro_y = gyro.gyro.y ;
            gyro_z = gyro.gyro.z ;
            client.print(accel_x);client.print(";");
            client.print(accel_y);client.print(";");
            client.print(accel_z);client.print(";");
            client.print(mag_x);client.print(";");
            client.print(mag_y);client.print(";");
            client.print(mag_z);client.print(";");
            client.print(gyro_x);client.print(";");
            client.print(gyro_y);client.print(";");
            client.print(gyro_z);client.println(";");
          }

          // repasser dans l'état 0 qui correspond à l'état de fonctionnement de base du robot
          if (msg == "etat_base") {
            Serial.println("etat de base");
            etat = 0;
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
          if (msg.substring(0,5) == "monte") { 
            int pos1 = msg.indexOf(";");
            //Serial.println(pos1);
            int pos2 = msg.indexOf(";",pos1+1);
            String chaine1 = msg.substring(0,pos1);
            String chaine2 = msg.substring(pos1+1,pos2);
            monte(taille_esc, nb_marche, chaine2.toInt(), accel , gyro , temp , mag);
          }

          // monte les escaliers
          if (msg == "change") { 
            tourne(accel , gyro , temp , mag);
          }



          // ------- afficher les données de l'ICM dans le but de les enregistrer en txt
          // ------- copier l'output et le coller dans un txt
          if (msg == "afficher_icm") {
            client.stop();
            while (1) {
              icm.getEvent(&accel, &gyro, &temp, &mag);
              accel_x = accel.acceleration.x ;
              accel_y = accel.acceleration.y ;
              accel_z = accel.acceleration.z ;
              mag_x = mag.magnetic.x ;
              mag_y = mag.magnetic.y ;
              mag_z = mag.magnetic.z ;
              gyro_x = gyro.gyro.x ;
              gyro_y = gyro.gyro.y ;
              gyro_z = gyro.gyro.z ;

              filter.update(
                gyro.gyro.x, gyro.gyro.y, gyro.gyro.z,
                accel.acceleration.x, accel.acceleration.y, accel.acceleration.z,
                mag.magnetic.x, mag.magnetic.y, mag.magnetic.z
              );

              Serial.print(accel_x);Serial.print(";");
              Serial.print(accel_y);Serial.print(";");
              Serial.print(accel_z);Serial.print(";");
              Serial.print(mag_x);Serial.print(";");
              Serial.print(mag_y);Serial.print(";");
              Serial.print(mag_z);Serial.print(";");
              Serial.print(gyro_x);Serial.print(";");
              Serial.print(gyro_y);Serial.print(";");
              Serial.print(gyro_z);Serial.print(";");
              Serial.print(filter.getRoll());Serial.print(";");
              Serial.print(filter.getPitch());Serial.print(";");
              Serial.print(filter.getYaw());Serial.println(";");
              delay(50);

              // si le gui envoie le mot stop, on arrête l'acquisition des données
            
              client = server.available();
              if (client) {
                Serial.println("stop");
                break;
              }
              delay(50);

            }
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

        }
        delay(10);
      }

      client.stop();
      Serial.println("Client deconnecte");
    }

    
    /* Envoyer en quasi continu l'état du robot (nb marche montées etc)
    */
    if (etat == 0) {
      // ------ comptage du nombre de tours de roue --------------
      //Serial.print( "nb_tr_avance  av ");
      //Serial.println(nb_tr_avance);
      nb_avance (nb_tr_gauche, nb_tr_droite, nb_tr_avance, etat, client, accel , gyro , temp , mag);
      //Serial.print( "nb_tr_avance  ap ");
      //Serial.println(nb_tr_avance);
      delay(10);
      //nb_gauche (accel , gyro , temp , mag);
      //nb_droite (accel , gyro , temp , mag);
    }

    delay(5);

    }
}
