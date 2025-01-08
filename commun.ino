#include "ArduinoJson.h"

#define DHTPIN 2 // Pin DHT11

uint8_t dht_data[5];  

void setup() {
  Serial.begin(9600);
  
  pinMode(DHTPIN, OUTPUT);
  digitalWrite(DHTPIN, HIGH);
  delay(1000);
}

void loop() {
  // Stockage de tous les résultats
  JsonDocument result;

  
  if (readDHT()) {
    result["TEMP_EXT"] = dht_data[2];
    result["HUM_EXT"] = dht_data[0];
  } else {
    result["TEMP_EXT"] = false;
    result["HUM_EXT"] = false;
  }


  // Afficher les données dans le serial
  serializeJson(result, Serial);
}

// Fonction pour lire les données du DHT11
bool readDHT() {
  uint8_t bitIndex = 0;
  uint8_t checksum;

  // Démarrer la communication avec le DHT11
  pinMode(DHTPIN, OUTPUT);
  digitalWrite(DHTPIN, LOW);  // Démarrer la séquence en mettant à LOW
  delay(18);  // Garder LOW pendant 18ms pour signaler le début de la transmission
  digitalWrite(DHTPIN, HIGH);  // Passer à HIGH
  delayMicroseconds(40);  // Attendre pendant 40µs
  
  // Passer en mode entrée pour recevoir la réponse du DHT11
  pinMode(DHTPIN, INPUT);

  // Attendre que le DHT11 réponde
  if (digitalRead(DHTPIN) == LOW) {
    delayMicroseconds(80);  // Attendre 80µs (réponse du DHT11)
    if (digitalRead(DHTPIN) == HIGH) {
      delayMicroseconds(80);  // Attendre encore 80µs (réponse du DHT11)
    } else {
      return false;  // Si la réponse est incorrecte, renvoyer false
    }
  } else {
    return false;  // Si la réponse ne vient pas au bon moment
  }

  // Lire les 40 bits de données
  for (int i = 0; i < 40; i++) {
    uint8_t bitValue = 0;
    while (digitalRead(DHTPIN) == LOW);  // Attendre que le signal devienne HIGH
    delayMicroseconds(30);  // Attendre 30µs pour lire la durée du signal
    if (digitalRead(DHTPIN) == HIGH) {
      bitValue = 1;  // Si le signal reste HIGH pendant plus de 30µs, c'est un 1
    }
    dht_data[i / 8] = (dht_data[i / 8] << 1) | bitValue;  // Stocker les bits dans le tableau
    while (digitalRead(DHTPIN) == HIGH);  // Attendre que le signal redevienne LOW
  }

  // Calculer le checksum
  checksum = dht_data[0] + dht_data[1] + dht_data[2] + dht_data[3];

  // Vérifier si le checksum est correct
  if (dht_data[4] == checksum) {
    return true;  // Si le checksum est correct, retourner true
  } else {
    return false;  // Si le checksum est incorrect, retourner false
  }
}
