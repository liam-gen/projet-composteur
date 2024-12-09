#include "Grove_Temperature_And_Humidity_Sensor.h"
#define DHTTYPE DHT11 

#define DHTPIN 2
DHT dht(DHTPIN, DHTTYPE);
#define debug  Serial


void setup() {

    debug.begin(115200);
    Wire.begin();

    dht.begin();
}

void loop() {
    float temp_hum_val[2] = {0};

    // Format JSON : {"type": "type_du_capteur", "value": valeur, "error": true/false}

    if (!dht.readTempAndHumidity(temp_hum_val)) {
        debug.println('{type: "CAPT_TEMP_HUM", value: ['+temp_hum_val[0]+', '+temp_hum_val[1]+'], error: false}');
        //debug.print("Humidity: ");
        //debug.print(temp_hum_val[0]);
        //debug.print(" %\t");
        //debug.print("Temperature: ");
        //debug.print(temp_hum_val[1]);
        //debug.println(" *C");
    } else {
        debug.println('{type: "CAPT_TEMP_HUM", value: [null, null], error: true}');
        //debug.println("Failed to get temprature and humidity value.");
    }

    delay(1500);
}
