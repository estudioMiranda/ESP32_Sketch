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

// Biblioteca para acceso a la fecha/hora por servidor NTP
#include <NTPClient.h>
#include <WiFiUdp.h>

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

// Tu ombre de la red y contraseña WiFi
const char* ssid     = "studiomiranda";
const char* password = "88888888";

// Definimos un Cliente NTP para obtener la fecha/hora
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

// Biblioteca de espera para llamar a una función
#include <Ticker.h>

// Variables para almacener la fecha/hora
String formattedDate;
String day;
String hour;
String timestamp;


// Variables para almacenar los datos del mensaje LoRa
int rssi;
String loRaMessage;
String temperature;
String humidity;
String pressure;
String readingID;

void enviarDatosSensor();

Ticker timer;

// Asignamos puertos para el servidor web y websocket
AsyncWebServer server(80);
WebSocketsServer websockets(81);

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
void startLoRA(){
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

void connectWiFi(){
  // Conexión Wi-Fi
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Muestra la IP local de la placa de desarrollo
  Serial.println("");
  Serial.println("WiFi conectado.");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
  display.setCursor(0,20);
  display.print("Dirección IP: ");
  display.setCursor(0,30);
  display.print(WiFi.localIP());
  display.display();
}

// Leyendo el paquete LoRa, con los datos del sensor BME280
void getLoRaData() {
  Serial.print("Paquete LoRa recibido: ");
  // Read packet
  while (LoRa.available()) {
    String LoRaData = LoRa.readString();
    // LoRaData format: readingID/temperature&soilMoisture#batterylevel
    // String example: 1/27.43&654#95.34
    Serial.print(LoRaData); 
    
    // Extrae los datos del sensor del mensaje
    int pos1 = LoRaData.indexOf('/');
    int pos2 = LoRaData.indexOf('&');
    int pos3 = LoRaData.indexOf('#');
    readingID = LoRaData.substring(0, pos1);
    temperature = LoRaData.substring(pos1 +1, pos2);
    humidity = LoRaData.substring(pos2+1, pos3);
    pressure = LoRaData.substring(pos3+1, LoRaData.length());    
  }
  // Obtiene el dato RSSI
  // Received Signal Strength Indicator
  // Indicador de fuerza de la señal recibida
  rssi = LoRa.packetRssi();
  Serial.print(" con RSSI ");    
  Serial.println(rssi);
}

// Función que obtiene fecha/hora del Cliente NTP
void getTimeStamp() {
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  // El formato que obtenemos es el siguente:
  // 2018-05-28T16:00:13Z
  
  formattedDate = timeClient.getFormattedDate();
  Serial.println(formattedDate);

  // Separamos la fecha/hora en dos variables
  int splitT = formattedDate.indexOf("T");
  day = formattedDate.substring(0, splitT);
  Serial.println(day);
  // Extract time
  hour = formattedDate.substring(splitT+1, formattedDate.length()-1);
  Serial.println(hour);
  timestamp = day + " " + hour;
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
      }
}

// Función envío datos por websocket en formato Json
void enviarDatosSensor() {
  
  String h = humidity;
  String t = temperature;
  String p = pressure;
  String d = timestamp;
  int r = rssi;
   
   String JSON_Data = "{\"temp\":";
          JSON_Data += t;
          JSON_Data += ",\"hum\":";
          JSON_Data += h;
          JSON_Data += ",\"pres\":";
          JSON_Data += p;
          JSON_Data += ",\"time\":";
          JSON_Data += "\"";
          JSON_Data += d;
          JSON_Data += "\"";
          JSON_Data += ",\"rssi\":";
          JSON_Data += r;
          JSON_Data += "}";
          
   Serial.println(JSON_Data);
   websockets.broadcastTXT(JSON_Data);  // envia datos a todos los clientes conectados
}


void setup() { 
  // Baudios para el monitor serie
  Serial.begin(115200);
  // Llama a las funiones pantalla, LoRa y WiFi
  startOLED();
  startLoRA();
  connectWiFi();
  
  if(!SPIFFS.begin()){
    Serial.println("Ha sucedido un error al iniciar SPIFFS");
    return;
  }
  //  Respuesta a la petición de "IP_local/"
    server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  { request->send(SPIFFS, "/index.html", "text.html");  
    });
    server.on("/winter", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/winter.jpg", "image/jpg");
    });

  // Inicio del servidor web y websocket
  server.begin();
  websockets.begin();
  websockets.onEvent(webSocketEvent);
  
  // Ticker timer (Llama funciones en un intervalo establecido)
  timer.attach(10,enviarDatosSensor);
   
  // Obtiene del Cliente la fecha/hora
  timeClient.begin();
  // Ajusta en setTimeOffset tu zona horaria en segundos
  // GMT +1 = 3600
  // GMT +2 = 7200 España/Madrid
  // GMT +8 = 28800
  // GMT -1 = -3600
  // GMT 0 = 0
  timeClient.setTimeOffset(7200);
}

void loop() {
  websockets.loop();
  // Comprueba si hay paquetes disponibles
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    getLoRaData();
    getTimeStamp();

  }
 }
