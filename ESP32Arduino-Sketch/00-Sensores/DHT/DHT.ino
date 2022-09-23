#include "DHT.h"

#define DHTPIN 25

#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHTxx test!"));

  dht.begin();
}

void loop() {
  delay(2000);
  
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  
  if (isnan(h) || isnan(t)) {
    Serial.println(F("¡Error de lectura del sensor DHT!"));
    return;
  }

  Serial.print(F("Temperatura: "));
  Serial.print(t);
  Serial.print(F("°C  Humedad: "));
  Serial.print(h);
  Serial.print(F("%"));
  Serial.println();
}
