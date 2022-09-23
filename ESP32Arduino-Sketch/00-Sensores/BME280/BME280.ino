#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C

unsigned long delayTime;

void setup() {
  Serial.begin(115200);
  Serial.println(F("BME280 test"));

  bool status;
  
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("¡Sensor BME280 no detectado, revisa las conexiones!");
    while (1);
  }

  Serial.println("-- Prueba del sensor --");
  delayTime = 1000;

  Serial.println();
}

void loop() { 
  printValues();
  delay(delayTime);
}

void printValues() {
  Serial.print("Temperatura = ");
  Serial.print(bme.readTemperature());
  Serial.println(" *C");
  
  Serial.print("Presión = ");
  Serial.print(bme.readPressure() / 100.0F);
  Serial.println(" hPa");

  Serial.print("Altitud aproximada = ");
  Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
  Serial.println(" m");

  Serial.print("Humedad = ");
  Serial.print(bme.readHumidity());
  Serial.println(" %");

  Serial.println();
}
