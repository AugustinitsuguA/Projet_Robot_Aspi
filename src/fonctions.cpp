#include "../include/fonctions.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_ICM20948.h>
#include <WiFi.h>
#include <ESP32Servo.h>

// pins du driver
#define MOTOR_D_IN1 10
#define MOTOR_D_IN2 11
#define MOTOR_G_IN1 12
#define MOTOR_G_IN2 13


void moteur(int pwm1 , int pwm2){
  if (pwm1 >= 0) {
      analogWrite(MOTOR_G_IN1 , 0);
      analogWrite(MOTOR_G_IN2 , pwm1);
  }
  else{
      analogWrite(MOTOR_G_IN1 , -pwm1);
      analogWrite(MOTOR_G_IN2 , 0);
  }
  if (pwm2 >= 0) {
      analogWrite(MOTOR_D_IN1 , 0);
      analogWrite(MOTOR_D_IN2 , pwm2);
  }
  else{
      analogWrite(MOTOR_D_IN1 , -pwm2);
      analogWrite(MOTOR_D_IN2 , 0);
  }
}


// monter les marches et arrêter le robot au bon endroit
void monte(WiFiClient &client,int taille_esc, int vitesse, sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag){
  float gyro_y;
  float gy;
  float gya4;
  float gya3;
  float gya2;
  float gya1;

  float gyro_y_av;
  int zone;
  int i;
  icm.getEvent(&accel, &gyro, &temp, &mag);
  i=0;
  zone = 0;
  delay(50);
  
  gy = gyro.gyro.y ;
  gya4 = -0.2;
  gya3 = -0.2;
  gya2 = -0.2;
  gya1 = -0.2;
  moteur(vitesse , vitesse);
  int nb_marche = 0;
  /*
  // fait les premières marches sans utiliser l imu
  delay(2000);
  moteur(0,0),
  nb_marche = nb_marche+1;
  delay(1000);

  // nouveau ---------------
  if (arret(client)) {
    moteur(0,0);
    return;
  }
  //envoie_donnees(client, 0, 0);
  // ----------------------
  
  moteur(vitesse , vitesse);
  delay(2000);
  moteur(0,0),
  nb_marche = nb_marche+1;
  delay(1000);

  // nouveau ----------------
  if (arret(client)) {
    moteur(0,0);
    return;
  }
  //envoie_donnees(client, 0, 0);
  // ----------------------
  */

  

  while (nb_marche < taille_esc-1){
    
    int delay_freq = 50; //ms

    moteur(vitesse , vitesse);

    icm.getEvent(&accel, &gyro, &temp, &mag);
    gy = - gyro.gyro.y ; // - parce que les valeurs se sont inversées : raison inconnue
    // si on se trouve dans la zone 1
    if (gya2>0 && (gy-gya2)<0.1 &&  gya2<=gya1 && gya2<=gy && gya2<=gya1 && gya1<=gy){
      zone = 1;
    }
    Serial.print("gyro");Serial.print(gy);Serial.print("stop");Serial.println(zone);
    Serial.print("1  ");Serial.print(gya1);
    Serial.print("1  ");Serial.print(gya2);
    gya4 = gya3;
    gya3 = gya2;
    gya2 = gya1;
    gya1 = gy;

    // si on était dans la zone 1 et que l'on retombe dans la zone 0
    if (zone==1 && gy <0) {
      Serial.print("stop");Serial.println(gy);
      zone = 0;
      moteur(0 ,0);
      delay(200);
      
      moteur(vitesse ,vitesse);
      nb_marche = nb_marche+1;
      //envoie_donnees(client, 0, 0);

      // avance jusqu'à être au niveau de la marche pour déployer le drone
      delay(2500);
      moteur(0 ,0);
      delay(3000);
    }

    if (arret(client)) {
      moteur(0,0);
      return;
    }

    delay(delay_freq); //nouveau
  }

  /*
  delay(4000);
  tourne(client, 2, accel, gyro, temp, mag);
  */
  moteur(0,0);
}

void plateforme(WiFiClient &client, Servo &servo_platforme, int vitesse, int temps){
  Serial.println("plateforme");
  Serial.print(vitesse);
  for (int i = 0; i<temps/50; i++){
    if (arret(client)) {
      moteur(0,0);
      return;
    }
    servo_platforme.write(vitesse);
    delay(50);
  }
  // arret de la plateforme
  servo_platforme.write(90);
}

int arret(WiFiClient &client){
  client = server.available();
    if (client) {
      return 1;
    }
    return 0;
}


void tourne(WiFiClient &client, int nb_tours, sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag){
  Serial.println("dedans");
  int i = 0;
  int nb = 0;
  int x_up;
  int x_up_p = 0;
  float gx;
  icm.getEvent(&accel, &gyro, &temp, &mag);
  gx = gyro.gyro.x ;
  moteur(0,118);

  while(nb<nb_tours){
    //Serial.println("oui");
    icm.getEvent(&accel, &gyro, &temp, &mag);
    gx = gyro.gyro.x ;

    if ((gx) > 0.7){
      x_up = 1;
    }
    if ((gx) < 0.7){
      x_up = 0;
    }

    // nouveau ---------------
    if (arret(client)) {
      moteur(0,0);
      return;
    }

    Serial.print("x up ");Serial.println(x_up);
    Serial.print("gx");Serial.println(gx);
    //Serial.print("y up ");Serial.println(y_up);

    if (((x_up == 0) && (x_up_p == 1))) {
      Serial.println("nb_tour + 1 _______");
      nb = nb+1;
      moteur(0,0);
      delay(500);
      moteur(0,118);
      
    }

    x_up_p = x_up;
    delay(100);
  }
  moteur(0,0);
}

// Avance contolee, but : ralentir avant que la pale de la roue touche, pour éviter les chocs.
void avance_controlee(WiFiClient &client, int vitesse_1, int vitesse_2, int vitesse_1D, int vitesse_2D, int temps_1, sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag)
{
  float gyro_x;
  float gyro_y;
  int x_up = 0;
  int x_up_p = 0;

    while (1){
      icm.getEvent(&accel, &gyro, &temp, &mag);
      gyro_y = gyro.gyro.y ;

      if (arret(client)) {
        moteur(0,0);
        return;
      }

      moteur(vitesse_1,vitesse_1D);
      delay(temps_1);

      // si le robot commence à pencher vers l'avant, on ralentit pour éviter le choc
      if (gyro_y < - 0.2){ // tester plusieurs valeurs
        moteur(vitesse_2,vitesse_2D);

        // tant qu'il n'y a pas eu le choc
        while (1){

          icm.getEvent(&accel, &gyro, &temp, &mag);
          gyro_x = gyro.gyro.x ;

          if (gyro_x > 0.42){
            x_up = 1;
          }
          if (gyro_x < 0.38){
            x_up = 0;
          }

          if ((x_up == 0) && (x_up_p == 1)) {
            Serial.println("nb_tour + 1 _______");
            delay(20);
            break;
          }

          if (arret(client)) {
            moteur(0,0);
            return;
          }

          x_up_p = x_up;

          delay(50);
        }

        // on attend pour que le robot se remette en place
        delay(2000);
        moteur(0,0);

        if (arret(client)) {
          moteur(0,0);
          return;
        }
        
        delay(2000);
        break;
      }
    }
}

void info_etat(WiFiClient &client, sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag){
  icm.getEvent(&accel, &gyro, &temp, &mag);
  float accel_x = accel.acceleration.x ;
  float accel_y = accel.acceleration.y ;
  float accel_z = accel.acceleration.z ;
  float mag_x = mag.magnetic.x ;
  float mag_y = mag.magnetic.y ;
  float mag_z = mag.magnetic.z ;
  float gyro_x = gyro.gyro.x ;
  float gyro_y = gyro.gyro.y ;
  float gyro_z = gyro.gyro.z ;
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

// Affiche les données de l'imu sur le moniteur série
void afficher_icm(WiFiClient &client, sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag){
  client.stop();
  while (1) {
    icm.getEvent(&accel, &gyro, &temp, &mag);
    float accel_x = accel.acceleration.x ;
    float accel_y = accel.acceleration.y ;
    float accel_z = accel.acceleration.z ;
    float mag_x = mag.magnetic.x ;
    float mag_y = mag.magnetic.y ;
    float mag_z = mag.magnetic.z ;
    float gyro_x = gyro.gyro.x ;
    float gyro_y = gyro.gyro.y ;
    float gyro_z = gyro.gyro.z ;

    Serial.print(accel_x);Serial.print(";");
    Serial.print(accel_y);Serial.print(";");
    Serial.print(accel_z);Serial.print(";");
    Serial.print(mag_x);Serial.print(";");
    Serial.print(mag_y);Serial.print(";");
    Serial.print(mag_z);Serial.print(";");
    Serial.print(gyro_x);Serial.print(";");
    Serial.print(gyro_y);Serial.print(";");
    Serial.print(gyro_z);Serial.println(";");

    // A tester
    /*
    envoie_donnees(client, 0, 0, accel, gyro, temp, mag);
    */

    delay(50);

    // si le gui envoie le mot stop, on arrête l'acquisition des données
  
    client = server.available();
    if (arret(client)) {
      moteur(0,0);
      return;
    }
    delay(50);

  }
}


void envoie_donnees(WiFiClient &client,  int etat, int marche, sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag){
  
  icm.getEvent(&accel, &gyro, &temp, &mag);
  float gyro_y = gyro.gyro.y ;
  client.print(gyro_y);client.print(";");
  client.print(etat);client.print(";");
  client.print(marche);client.println(";");
}

void monte_plateforme(){
  
}
// compter les nombre de tours de roue quand avance 
void nb_avance (int &nb_tr_gauche, int &nb_tr_droite, int &nb_tr_avance, int &etat, WiFiClient client, sensors_event_t &accel, sensors_event_t &gyro,sensors_event_t &temp,sensors_event_t &mag){

  int x_up;
  int y_up;
  int x_up_p; 
  int y_upD;
  int y_up_pD;
  int y_upG;
  int y_up_pG;
  int y_up_p;
  float accel_x;
  float accel_y;
  float accel_z;
  float gyro_x;
  float gyro_y;
  float gyro_z;
  Serial.println("Avance");
  
  x_up_p = 0 ;
  y_up_p = 0 ;
  while (etat == 0) {
    icm.getEvent(&accel, &gyro, &temp, &mag);
    accel_x = accel.acceleration.x ;
    accel_y = accel.acceleration.y ;
    accel_z = accel.acceleration.z ;

    gyro_x = gyro.gyro.x ;
    gyro_y = gyro.gyro.y ;
    gyro_z = gyro.gyro.z ;

    /*
    //Serial.println(accel_z);
    if (accel_x < -4.5){
      x_up = 1;
    }
    if (accel_x > -4.5){
      x_up = 0;
    }

    // GAUCHE
    if (accel_y > 1.5){
      y_upG = 1;
    }
    if (accel_y < 1.2){
      y_upG = 0;
    }

    // DROITE
    if (accel_y < -1.5){
      y_upD = 1;
    }
    if (accel_y > -1.2){
      y_upD = 0;
    }
    */

    Serial.print("gyro_x ");Serial.println(gyro_x);
    Serial.print("gyro_y ");Serial.println(gyro_y);

    if ((gyro_x) > 0.5){
      x_up = 1;
    }
    if ((gyro_x) < 0.5){
      x_up = 0;
    }

    if ((gyro_y) > 0.7){
      y_up = 1;
    }
    if ((gyro_y) < 0.7){
      y_up = 0;
    }

    //Serial.print("x up ");Serial.println(x_up);
    //Serial.print("y up ");Serial.println(y_up);

    if (((x_up == 0) && (x_up_p == 1)) || ((y_up == 0) && (y_up_p == 1))) {
      nb_tr_avance = nb_tr_avance + 1 ;
      Serial.println("nb_tour + 1 _______");
      delay(20);
    }

/*
    if ((y_up_pG == 1) && (y_upG == 0)) {
      nb_tr_gauche = nb_tr_gauche + 1 ;
      Serial.println("GA ------------");
    }

    if ((y_up_pD == 1) && (y_upD == 0)) {
      nb_tr_droite = nb_tr_droite + 1 ;
      Serial.println("DR -------------");
    }
*/

    x_up_p = x_up;
    y_up_p = y_up;
    y_up_pG = y_upG;
    y_up_pD = y_upD;

    //Serial.println(accel_y);
    Serial.print(nb_tr_avance);
    Serial.print(nb_tr_gauche);
    Serial.print(nb_tr_droite);
    Serial.println();
    delay(50);


    // stop si on appuie sur un bouton
    client = server.available();
    if (client) {
      // si la fonction appelée est la demande de recup de valeurs
      String msg = client.readStringUntil('\n');
      msg.trim();
      if (msg == "nb_tours") {
        Serial.print("demande");
        client.print(nb_tr_avance);client.print(";");
        client.print(nb_tr_gauche);client.print(";");
        client.print(nb_tr_droite);client.println(";");
      }
      else if (msg.startsWith("m1")) {
        moteur(0 , 0);
      }
      else  { // sinon stopper la boucle
        Serial.println("stop");
        etat = -1;


        break;
      }  
    }
    //delay(50);
  }

}


