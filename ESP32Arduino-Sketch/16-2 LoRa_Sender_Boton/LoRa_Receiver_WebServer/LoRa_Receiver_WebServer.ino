/*********
  StudioMiranda Arduinoes
  WebSocket ServidorWeb
*********/
#include <ArduinoJson.h>
//  Bibliotaca comunicación LoRa
#include <SPI.h>
#include <LoRa.h>

//  Biblietecas pantalla OLED "SSD1306"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Pines transmisión/recepción TTGO LoRa v.1.0
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//  433E6 Banda Asia
//  866E6 Banda Europa
//  915E6 Banda Norteamérica
#define BAND 866E6

// Pines pantalla OLED 
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // Pantalla OLED ancho, en pixeles
#define SCREEN_HEIGHT 64 // Pantalla OLED alto, en pixeles

#define led 2

// Variables para almacenar los datos del mensaje LoRa
int rssi;
String loRaMessage;
String readingID;


// Instancia de la pantalla
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// Inicia la pantalla OLED
void startOLED(){
  // Reinicio pantalla
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  // Inicio pantalla
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("Fallo de incio pantalla SSD1306"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Receptor LoRa");
}

// Inicio del módulo emisor/receptor LoRa
void startLoRa(){
  int counter;
  // Pines SPI LoRa
  SPI.begin(SCK, MISO, MOSI, SS);
  
  // Configuración módulo
  LoRa.setPins(SS, RST, DIO0);

  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) {
    // Incrementa el readingID
    Serial.println("¡Fallo de inicio LoRa!"); 
  }
  Serial.println("¡LoRa iniciado, OK!");
  display.setCursor(0,10);
  display.clearDisplay();
  display.print("¡LoRa iniciado, OK!");
  display.display();
  delay(2000);
}

// Leyendo el paquete LoRa, con los datos del sensor BME280
void getLoRaData() {
  Serial.print("Paquete LoRa recibido: ");
  // Read packet
  while (LoRa.available()) {
    String LoRaData = LoRa.readString();
    Serial.print(LoRaData);

    DynamicJsonDocument doc(200); // documento (capacidad)
    DeserializationError error = deserializeJson(doc, LoRaData);
     if (error) {
       Serial.print("deserializeJson() failed: ");
       Serial.println(error.c_str());
       return;
      }
        
      int estadoLed = doc["Led"]; // el estado será de 0 o 1
      digitalWrite(led, estadoLed);
      }
  // Obtiene el dato RSSI
  // Received Signal Strength Indicator
  // Indicador de fuerza de la señal recibida
  rssi = LoRa.packetRssi();
  Serial.print(" con RSSI ");    
  Serial.println(rssi);     
}

void setup() { 
  // Baudios para el monitor serie
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  // Llama a las funiones pantalla, LoRa y WiFi
  startOLED();
  startLoRa();
}
  
void loop() {
  // Comprueba si hay paquetes disponibles
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    getLoRaData();
  }

 }
