#include "ArduinoJson.h"
#include "Grove_Temperature_And_Humidity_Sensor.h"
#include "MutichannelGasSensor.h"

#define DHTTYPE DHT11 

#define DHTPIN 2
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(115200);

    // Humidité
    Wire.begin();
    dht.begin();

    // Début du gaz

    gas.begin(0x04);
    gas.powerOn();
}

// Format JSON : {"type": "type_du_capteur", "value": valeur, "error": true/false}

JsonDocument getHumidityAndTemperature(){
  JsonDocument result;

  float temp_hum_val[2] = {0};

  result["error"] = false;
  result["type"] = "CAPT_TEMP_HUM";

  if (!dht.readTempAndHumidity(temp_hum_val)) {
      result["value"]["humidity"] = temp_hum_val[0];
      result["value"]["temperature"] = temp_hum_val[1];
  } else {
      result["error"] = true;
      result["value"] = NULL;
  }

  return result;
}

JsonDocument getGas(){
  JsonDocument result;
  float c;

  result["error"] = false;
  result["type"] = "CAPT_GAS";

  c = gas.measure_CO();
  if(c>=0) result["value"]["co"] = c;

  return result;
}

void loop() {
  serializeJson(getGas(), Serial);
  Serial.println("");
  serializeJson(getHumidityAndTemperature(), Serial);
  Serial.println("");
}
