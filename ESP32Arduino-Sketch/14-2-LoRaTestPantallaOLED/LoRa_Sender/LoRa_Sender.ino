/*********
  Modificado por StudioMiranda Arduinoes
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/ttgo-lora32-sx1276-arduino-ide/
*********/

// Bibliotecas para LoRa
#include <SPI.h>
#include <LoRa.h>

// Bibliotecas para la pantalla OLED
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pines del módulo de emisor/receptor LoRa
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
#define SCREEN_WIDTH 128 // Ancho pantalla OLED, en pixeles
#define SCREEN_HEIGHT 64 // Alto pantalla OLED, en pixeles

// Contador de paquetes
int counter = 0;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  // Baudios monitor serie
  Serial.begin(115200);

  //Reinicio de la pantalla OLED
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  // Inicio pantalla OLED
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Dirección 0x3C para 128x32
    Serial.println(F("Fallo de asignación SSD1306"));
    for(;;); // No continuar, bucle infinito
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Emisor LoRa");
  display.display();
  
  Serial.println("Test emisor LoRa");

  //Pines SPI LoRa
  SPI.begin(SCK, MISO, MOSI, SS);
  
  // Configuración del módulo LoRa
  LoRa.setPins(SS, RST, DIO0);
  
  if (!LoRa.begin(BAND)) {
    Serial.println("¡Fallo de inicio LoRa!");
    while (1);
  }
  Serial.println("¡Iniciando LoRa!");
  display.setCursor(0,10);
  display.print("LoRa iniciado!");
  display.display();
  delay(2000);
}

void loop() {

  digitalWrite(LED_BUILTIN, HIGH);
   
  Serial.print("Enviando paquetes: ");
  Serial.println(counter);

  //Envío de paquetes LoRa
  LoRa.beginPacket();
  LoRa.print("Hola ");
  LoRa.print(counter);
  LoRa.endPacket();
  Serial.println(LoRa.parsePacket());
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Emisor LoRa");
  display.setCursor(0,20);
  display.setTextSize(1);
  display.print("Paquete enviado LoRa");
  display.setCursor(0,30);
  display.print("Contador:");
  display.setCursor(50,30);
  display.print(counter);      
  display.display();

  counter++;
  
  delay(10000);
}
