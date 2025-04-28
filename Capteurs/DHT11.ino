#define DHTPIN 2
uint8_t data[5];  

void setup() {
  Serial.begin(9600);
  
  pinMode(DHTPIN, OUTPUT);
  digitalWrite(DHTPIN, HIGH);
  delay(1000);
}

void loop() {
   getHumAndTemp();

   int humidite = data[0];
   int temp = data[2];
   Serial.print(temp);
   Serial.println(" °C");
   Serial.print(humidite);
   Serial.println(" %");
   delay(5000);
}

void getHumAndTemp()
{
  // Démarrer la communication avec le DHT11
  pinMode(DHTPIN, OUTPUT);
  digitalWrite(DHTPIN, LOW);
  delay(18);
  digitalWrite(DHTPIN, HIGH);
  delayMicroseconds(40);
  
  // Passer en mode entrée pour recevoir la réponse du DHT11
  pinMode(DHTPIN, INPUT);

  // Attendre que le DHT11 réponde
  if (digitalRead(DHTPIN) == LOW) {
    delayMicroseconds(80);
    if (digitalRead(DHTPIN) == HIGH) {
      delayMicroseconds(80);
    } else {
      return false;
    }
  } else {
    return false;
  }

  // Lire les 40 bits de données
  for (int i = 0; i < 40; i++) {
    uint8_t bitValue = 0;
    while (digitalRead(DHTPIN) == LOW);
    delayMicroseconds(30);  // Durée du signal
    if (digitalRead(DHTPIN) == HIGH) {
      bitValue = 1;
    }
    data[i / 8] = (data[i / 8] << 1) | bitValue;
    while (digitalRead(DHTPIN) == HIGH);
  }

}
