
// Bibliotecas
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <SPIFFS.h>

// Comunicación UART Serial2 
#define RXD2 16
#define TXD2 17

AsyncWebServer server(80); 
WebSocketsServer websockets(81);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Página no encontrada");
}

// Función que obtiene el paquete/mensaje JSON "payload"
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
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
      String mensaje = String((char*)( payload));
      
      // Envía el mensaje a la otra placa ESP32
      // Conexión UART a través de los pines TX2 y RX2
      Serial2.println(mensaje);
  }
}

void setup() {
  Serial.begin(115200);
  // Configuración comunicación UART
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  
  WiFi.softAP("PuntoAcceso", "88888888");
  Serial.println("soft Access Point: ");
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
}

void loop()
{   
  websockets.loop(); 
}
