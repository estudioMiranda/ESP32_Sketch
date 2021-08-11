
#include <esp_now.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include <WebSocketsServer.h>
#include <Arduino_JSON.h>
#include "SPIFFS.h"
#include <Ticker.h>

// Introduce tu conexión WiFi
const char* ssid = "studiomiranda";
const char* password = "88888888";

// Estructura del mensaje
typedef struct struct_message {
  int id;
  float temp;
  float hum;
  unsigned int readingId;
} struct_message;

struct_message incomingReadings;

JSONVar board;

AsyncWebServer server(80);
WebSocketsServer websockets(81);

// Función que recibe el paquete/mensaje/datos
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) { 
  // Dirección MAC del emisor
  char macStr[18];
  Serial.print("Paquete recibido de: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&incomingReadings, incomingData, sizeof(incomingReadings));
  
  board["id"] = incomingReadings.id;
  board["temperature"] = incomingReadings.temp;
  board["humidity"] = incomingReadings.hum;
  board["readingId"] = String(incomingReadings.readingId);
  String jsonString = JSON.stringify(board);
  websockets.broadcastTXT(jsonString.c_str()); 
  
  Serial.printf("Identificador de placa %u: %u bytes\n", incomingReadings.id, len);
  Serial.printf("Valor temperatura: %4.2f \n", incomingReadings.temp);
  Serial.printf("Valor humedad: %4.2f \n", incomingReadings.hum);
  Serial.printf("readingID value: %d \n", incomingReadings.readingId);
  Serial.println();
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Configuramos la conexión WiFi como A.P. y ESTACIÓN
  WiFi.mode(WIFI_AP_STA);
  
  // Configuración ESTACIÓN
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("WiFi configurado como ESTACIÓN..");
  }
  Serial.print("Dirección IP ESTACIÓN: ");
  Serial.println(WiFi.localIP());
  Serial.print("Canal Wi-Fi: ");
  Serial.println(WiFi.channel());

  if(!SPIFFS.begin(true)){
    Serial.println("A ocurrido un error al montar SPIFFS");
    return;
  }

  // Conexión ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("A ocurrido un error al iniciar ESP-NOW");
    return;
  }
  
  // Registra los paquetes recibidos
  esp_now_register_recv_cb(OnDataRecv);

  // Envía el archivo index.html a la petición de la IP
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request){
  request->send(SPIFFS, "/index.html", "text/html");
  });
  
  server.on("/winter", HTTP_GET, [](AsyncWebServerRequest *request){
  request->send(SPIFFS, "/winter.jpg", "image/jpg");
  });

  server.begin();
  websockets.begin();
}

// Enviamos un ping cada 5 segundos Servidor/Cliente
void loop() {
  websockets.loop();
}
