#include <EEPROM.h>

#define SensorPin A2       
#define LED 13             
#define samplingInterval 20
#define printInterval 800
#define ArrayLenth 40      
#define uart Serial        

float voltage_4, voltage_7;   // Tensions mesurées pour les pH de calibration
float slope, intercept;       // Coefficients de la droite de calibration

int pHArray[ArrayLenth];    
int pHArrayIndex = 0;

void setup() {
  pinMode(LED, OUTPUT);
  uart.begin(9600);
  uart.println("pH Meter with Calibration Mode (Send 'calibrate' in Serial Monitor)");

  loadCalibration();  // Charger la calibration stockée
}

void loop() {
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
  static float pHValue, voltage;

  if (uart.available()) {  // Vérifier si une commande est reçue
    String command = uart.readStringUntil('\n');
    command.trim();
    if (command == "calibrate") {
      calibrate();
    }
  }

  if (millis() - samplingTime > samplingInterval) {
    pHArray[pHArrayIndex++] = analogRead(SensorPin);
    if (pHArrayIndex == ArrayLenth) pHArrayIndex = 0;

    voltage = avergearray(pHArray, ArrayLenth) * 5.0 / 1024;
    pHValue = slope * voltage + intercept;  

    samplingTime = millis();
  }

  if (millis() - printTime > printInterval) {
    uart.print("Voltage: ");
    uart.print(voltage, 2);
    uart.print("    pH Value: ");
    uart.println(pHValue, 2);

    digitalWrite(LED, digitalRead(LED) ^ 1);
    printTime = millis();
  }
}

// Fonction de calibration
void calibrate() {
  uart.println("Calibration Mode: Place probe in pH 4.0 solution, then type 'pH4' and press Enter.");
  waitForCommand("pH4");
  voltage_4 = analogRead(SensorPin) * 5.0 / 1024;
  uart.print("pH 4.0 Voltage: ");
  uart.println(voltage_4, 3);

  uart.println("Now place probe in pH 7.0 solution, then type 'pH7' and press Enter.");
  waitForCommand("pH7");
  voltage_7 = analogRead(SensorPin) * 5.0 / 1024;
  uart.print("pH 7.0 Voltage: ");
  uart.println(voltage_7, 3);

  slope = (7.0 - 4.0) / (voltage_7 - voltage_4);
  intercept = 7.0 - (slope * voltage_7);

  saveCalibration();
  
  uart.println("Calibration complete!");
  uart.print("Slope: ");
  uart.println(slope, 3);
  uart.print("Intercept: ");
  uart.println(intercept, 3);
}

// Attente d'une commande spécifique dans le moniteur série
void waitForCommand(String expectedCommand) {
  while (true) {
    if (uart.available()) {
      String command = uart.readStringUntil('\n');
      command.trim();
      if (command == expectedCommand) return;
    }
  }
}

// Sauvegarde des valeurs de calibration dans l'EEPROM
void saveCalibration() {
  EEPROM.put(0, voltage_4);
  EEPROM.put(sizeof(float), voltage_7);
  uart.println("Calibration values saved.");
}

// Chargement des valeurs de calibration depuis l'EEPROM
void loadCalibration() {
  EEPROM.get(0, voltage_4);
  EEPROM.get(sizeof(float), voltage_7);

  if (voltage_4 == 0 || voltage_7 == 0) {
    uart.println("No calibration data found. Use 'calibrate' to calibrate.");
    slope = -19.18518519; 
    intercept = 41.02740741;
  } else {
    slope = (7.0 - 4.0) / (voltage_7 - voltage_4);
    intercept = 7.0 - (slope * voltage_7);
    uart.println("Calibration data loaded.");
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
