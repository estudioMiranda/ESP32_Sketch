/*********
  StudioMiranda Arduinoes
  WebSocket ServidorWeb
*********/
// Importa Biblioteca Wi-Fi, WebSocket y ServidorWeb
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <WebSocketsServer.h>

// Biblioteca comunicación con la memoria
#include <SPIFFS.h>

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

//  Contador de paquetes (ID)
int readingID = 0;
int counter = 0;
String LoRaMensaje = "";
String mensaje= "" ;

// Asignamos puertos para el servidor web y websocket√LoRa_Sender_Boton11
AsyncWebServer server(80);
WebSocketsServer websockets(81);

// Instancia de pantalla OLED
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
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Emisor LoRa");
}

// Inicio del módulo emisor/receptor LoRa
void startLoRa(){
  // Pines SPI LoRa
  SPI.begin(SCK, MISO, MOSI, SS);
  
  // Configuración módulo
  LoRa.setPins(SS, RST, DIO0);

  // Espera 5 segundos
  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) {
    // Incrementa el readingID
    readingID++;
    Serial.println("¡Fallo de inicio LoRa!"); 
  }
  Serial.println("¡LoRa iniciado, OK!");
  display.setCursor(0,10);
  display.clearDisplay();
  display.print("¡LoRa iniciado, OK!");
  display.display();
  delay(2000);
}

// Envia los datos en paquetes/mensajes
void sendReadings() {
  
  Serial.print("Enviando datos: ");
// Envío del mensaje
  LoRa.beginPacket();
  LoRa.print(mensaje);
  LoRa.endPacket();
  Serial.print(mensaje);

  // Muestra en pantalla OLED los datos
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.print("¡Paquete LoRa enviado!");
  display.setCursor(0,20);
}

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Página no encontrada");
}

// Función de conexión websocket
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) 
  {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] ¡Desconectado!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = websockets.remoteIP(num);
        Serial.printf("[%u] Conectado en %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
      }
      break;
      
     case WStype_TEXT:
      Serial.printf("[%u] Texto: %s\n", num, payload);
      mensaje = String((char*)( payload));
      Serial.println(mensaje);
      sendReadings();
  }
      
}

void setup() {
  // Baudios monitor serie
  Serial.begin(115200);
  
  WiFi.softAP("PuntoAcceso", "");
  Serial.print("softAP: ");
  Serial.println(WiFi.softAPIP());
  
  if(!SPIFFS.begin(true)){
    Serial.println("A ocurrido un error al montar SPIFFS");
    return;
  }

   server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
   request->send(SPIFFS, "/index.html", "text/html");
  });
  
  server.onNotFound(notFound);
  server.begin();
  
  websockets.begin();
  websockets.onEvent(webSocketEvent);
  
  // LLamada funciones pantalla, sensor y módulo LoRa
  startOLED();
  startLoRa();
} 
 
void loop() {
  websockets.loop();
}
