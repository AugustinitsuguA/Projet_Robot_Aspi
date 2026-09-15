#include <Arduino.h>
#include <WiFi.h>

#define LED_BUILTIN 2

#define TRIG_PIN 32
#define ECHO_PIN 33

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);

    Serial.println("ça marche");

    pinMode(TRIG_PIN, OUTPUT);
    pinMode(ECHO_PIN, INPUT);
}

void loop() {

    // On s'assure que TRIG est à LOW
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);

    // Impulsion de 10 µs
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);

    // Mesure de la durée de l'écho
    long duration = pulseIn(ECHO_PIN, HIGH);

    // Calcul de la distance en cm
    float distance = duration * 0.0343 / 2;

    Serial.print("Distance : ");
    Serial.print(distance);
    Serial.println(" cm");

    delay(200);
}
