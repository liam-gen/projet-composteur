#include "ArduinoJson.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include "MutichannelGasSensor.h"

/* DHT11 CONFIG */
#define DHTPIN 2
uint8_t dht_data[5];

/* DS18B20 CONFIG */
#define ONE_WIRE_BUS 7
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

/* Niveau eau */
int PIN_REMPLISSAGE = A0;

/* Moisture sensor */

int PIN_MOISTURE = A2;

/* Etat du moteur */

int PIN_MOTEUR = 8;

/* CO?FIG PHMETRE */

#include <EEPROM.h>

#define SensorPin A1
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth 40
#define uart Serial

float voltage_4 = 0.46;
float voltage_7 = 1.82;
float slope, intercept;

int pHArray[ArrayLenth];    
int pHArrayIndex = 0;

void setup() {
  Serial.begin(9600);
  
  /* DS18B20 CONFIG */
  sensors.begin();

  /* PHMETRE */
  loadCalibration();

  /* GAS CONFIG */
  gas.begin(0x04);
  gas.powerOn();

  /* Moteur */
  pinMode(PIN_MOTEUR, INPUT);

  /* DHT11 CONFIG */
  pinMode(DHTPIN, OUTPUT);
  digitalWrite(DHTPIN, HIGH);
  delay(1000);
}

void loop() {
  // Stockage de tous les résultats
  JsonDocument result;

  /* DHT11 DATA */
  if (readDHT()) {
    result["TEMP_EXT"] = dht_data[2];
    result["HUM_EXT"] = dht_data[0];
  } else {
    result["TEMP_EXT"] = false;
    result["HUM_EXT"] = false;
  }

  result["TEMP_INT"] = getIntTemperature();

  result["REMPLISSAGE"] = getRemplissage();

  result["MOISTURE"] = getMoisture();

  result["MOTOR"] = digitalRead(PIN_MOTEUR);

  // Amoniac
    result["AMONIAC"] = gas.measure_NH3();

    // Méthane
    result["METHANE"] = gas.measure_CH4();

  //result["PH"] = getPhValue();

  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue, voltage;

  if (millis() - samplingTime > samplingInterval) {
    pHArray[pHArrayIndex++] = analogRead(SensorPin);
    if (pHArrayIndex == ArrayLenth) pHArrayIndex = 0;

    //Serial.println(analogRead(SensorPin));

    voltage = avergearray(pHArray, ArrayLenth) * 5.0 / 1024;
    pHValue = slope * voltage + intercept;  

    samplingTime = millis();
  }

  if (millis() - printTime > printInterval) {
    printTime = millis();

    result["PH"] = pHValue;

    // Afficher les données en serial

    serializeJson(result, Serial);
    Serial.println("");
  
    delay(1000);
  }
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

int getIntTemperature(){

  sensors.requestTemperatures(); // Send the command to get temperatures
  float tempC = sensors.getTempCByIndex(0);
  
  if(tempC != DEVICE_DISCONNECTED_C) 
  {
    return tempC;
  } 

  return false;
}

float getRemplissage()
{
  int data = analogRead(PIN_REMPLISSAGE);

  // Convertir tension
  float v = (5.0/1023)*data;

  // Convertir en pourcentage
  float pourcent = 100*v/5;
  return pourcent;
}

float getMoisture()
{

  int N = analogRead(PIN_MOISTURE);
  
  //conversion information capteur vers tension
  int V = N*(5.0/1023);
  
  //conversion tension vers humidité
  int H = V /(3.26/100);

  return H;
}

/* PHMETRE */

float getPhValue()
{
  //Serial.println(analogRead(SensorPin));
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue, voltage;

  if (millis() - samplingTime > samplingInterval) {
    pHArray[pHArrayIndex++] = analogRead(SensorPin);
    if (pHArrayIndex == ArrayLenth) pHArrayIndex = 0;

    //Serial.println(analogRead(SensorPin));

    voltage = avergearray(pHArray, ArrayLenth) * 5.0 / 1024;
    pHValue = slope * voltage + intercept;  
    Serial.println(voltage);

    samplingTime = millis();
  }

  if (millis() - printTime > printInterval) {
    printTime = millis();

    return pHValue;
  }

  
}

// Chargement des valeurs de calibration depuis l'EEPROM
void loadCalibration() {
  if (voltage_4 == 0 || voltage_7 == 0) {
    slope = -19.18518519; 
    intercept = 41.02740741;
  } else {
    slope = (7.0 - 4.0) / (voltage_7 - voltage_4);
    intercept = 7.0 - (slope * voltage_7);
  }
}

// Fonction de filtrage des valeurs extrêmes (inchangée)
double avergearray(int* arr, int number) {
  int i;
  int max, min;
  double avg;
  long amount = 0;

  if (number <= 0) {
    uart.println("Error: Invalid array size.");
    return 0;
  }
  if (number < 5) { 
    for (i = 0; i < number; i++) {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  } else {
    if (arr[0] < arr[1]) {
      min = arr[0]; max = arr[1];
    } else {
      min = arr[1]; max = arr[0];
    }
    for (i = 2; i < number; i++) {
      if (arr[i] < min) {
        amount += min;
        min = arr[i];
      } else if (arr[i] > max) {
        amount += max;
        max = arr[i];
      } else {
        amount += arr[i];
      }
    }
    avg = (double)amount / (number - 2);
  }
  return avg;
}
