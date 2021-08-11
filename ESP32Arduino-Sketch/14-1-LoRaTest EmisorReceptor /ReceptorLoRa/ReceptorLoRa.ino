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

void setup() {
  //Pines SPI LoRa
  SPI.begin(SCK, MISO, MOSI, SS);
  
  // Configuración del módulo LoRa
  LoRa.setPins(SS, RST, DIO0);
  
  Serial.begin(9600);
  while (!Serial);

  Serial.println("Receptor LoRa");

  if (!LoRa.begin(866E6)) {
    Serial.println("Fallo al iniciar LoRa!");
    while (1);
  }
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Paquete recibido '");

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("' con RSSI ");
    Serial.println(LoRa.packetRssi());
  }
}
