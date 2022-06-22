#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme;

unsigned long delayTime;

void setup() {
  Serial.begin(115200);
  Serial.println(F("Prueba BME280"));

  bool status;
  
  status = bme.begin(0x76);  
  if (!status) {
    Serial.println("No se detecta sensor, verifique los cables");
    while (1);
  }

  Serial.println("-- Prueba por defecto --");
  delayTime = 5000;

  Serial.println();
}


void loop() { 
  printValues();
  delay(delayTime);
}

void printValues() {
  Serial.print("Temperatura = ");
  Serial.print(bme.readTemperature());
  Serial.println(" ªC");
  
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
