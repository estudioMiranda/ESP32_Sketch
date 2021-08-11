/*  Placa de desarrollo TTGO LoRa32-OLED V2.1.6
    Por StudioMiranda ArduinoES       */

#include <SPI.h>
#include <LoRa.h>

// Pines del módulo de emisor/receptor LoRa
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

int counter = 0;

void setup() {
  
  //Pines SPI LoRa
  SPI.begin(SCK, MISO, MOSI, SS);
  
  // Configuración del módulo LoRa
  LoRa.setPins(SS, RST, DIO0);
  
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Emisor LoRa ");

  

  if (!LoRa.begin(866E6)) {
    Serial.println("Fallo al iniciar LoRa!");
    while (1);
  }
}

void loop() {
  Serial.print("Enviando paquete: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print("Hola ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(5000);
}
