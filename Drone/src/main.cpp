#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>
#include <esp_wifi.h>
#include <esp_idf_version.h>

#define LED_BUILTIN 2

#define TRIG_PIN 32
#define ECHO_PIN 33

void afficher_message_espnow(const uint8_t *data, int longueur) {
    Serial.print("Message ESP-NOW reçu : ");
    Serial.write(data, longueur);
    Serial.println();
}

#if ESP_IDF_VERSION_MAJOR >= 5
void recevoir_espnow(const esp_now_recv_info_t *, const uint8_t *data, int longueur) {
#else
void recevoir_espnow(const uint8_t *, const uint8_t *data, int longueur) {
#endif
    afficher_message_espnow(data, longueur);
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(115200);

    Serial.println("ça marche");

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    if (esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE) != ESP_OK) {
        Serial.println("Erreur : impossible de regler le canal Wi-Fi ESP-NOW");
    } else if (esp_now_init() != ESP_OK) {
        Serial.println("Erreur : initialisation ESP-NOW impossible");
    } else if (esp_now_register_recv_cb(recevoir_espnow) != ESP_OK) {
        Serial.println("Erreur : impossible d'enregistrer le callback ESP-NOW");
    } else {
        Serial.println("Ecoute ESP-NOW active sur le canal 1");
    }

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
