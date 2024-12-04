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

    if (!dht.readTempAndHumidity(temp_hum_val)) {
        debug.print("Humidity: ");
        debug.print(temp_hum_val[0]);
        debug.print(" %\t");
        debug.print("Temperature: ");
        debug.print(temp_hum_val[1]);
        debug.println(" *C");
    } else {
        debug.println("Failed to get temprature and humidity value.");
    }

    delay(1500);
}
