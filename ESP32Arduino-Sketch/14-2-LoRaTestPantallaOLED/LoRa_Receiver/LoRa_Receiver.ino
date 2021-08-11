/*********
  Modificado por StudioMiranda ArduinoES
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/ttgo-lora32-sx1276-arduino-ide/
*********/

  // Bibliotacas para LoRa
#include <SPI.h>
#include <LoRa.h>

  // Bibliotecas para la pantalla OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

  // Pines para el módulo de transmisión
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

  //433E6 Asia
  //866E6 Europe
  //915E6 North America
#define BAND 866E6

  // Pines pantalla OLED
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED ancho de pantalla, en pixels
#define SCREEN_HEIGHT 64 // OLED alto de pantalla, en pixels

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

String LoRaData;

void setup() { 
  // Serial Monitor
  Serial.begin(115200);
  
  //reset OLED display via software
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);
  
  //initialize OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("Fallo no se encuentra SSD1306"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Receptor LoRa");
  display.display();

  Serial.println("Test receptor LoRa");
  
  // Pines SPI LoRa
  SPI.begin(SCK, MISO, MOSI, SS);
  
  // Configuración del modulo LoRa
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(BAND)) {
    Serial.println("¡Fallo de inicio LoRa!");
    while (1);
  }
  Serial.println("LoRa iniciado");
  display.setCursor(0,10);
  display.println("LoRa iniciado");
  display.display();  
}

void loop() {

  // Recibiendo el paquete
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Paquete recibido
    Serial.print("Paquete recibido: ");

    //Leyendo paquete
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      Serial.println(LoRaData);
    }

    // Mostrando el RSSI del paquete
    int rssi = LoRa.packetRssi();
    Serial.print("Con RSSI ");    
    Serial.println(rssi);

   // Información en la pantalla
   display.clearDisplay();
   display.setCursor(0,0);
   display.print("Receptor LoRa");
   display.setCursor(0,20);
   display.print("Paquete recibido:");
   display.setCursor(0,30);
   display.print(LoRaData);
   display.setCursor(0,40);
   display.print("RSSI:");
   display.setCursor(30,40);
   display.print(rssi);
   display.display();   
  }
}
