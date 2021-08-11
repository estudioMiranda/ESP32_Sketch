
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <WebSocketsServer.h>

#define Led 2

AsyncWebServer server(80); 
WebSocketsServer websockets(81);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "¡Página no encontrada!");
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) 
  {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] ¡Desconectado!\n", num);
      break;
    case WStype_CONNECTED: {
        IPAddress ip = websockets.remoteIP(num);
        Serial.printf("[%u] Conectado en %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        websockets.sendTXT(num, "Conectado en servidor:");
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] Recibe el texto: %s\n", num, payload);
      String mensaje = String((char*)( payload));
      Serial.println(mensaje);

      if(mensaje == "Led esta en OFF"){
        digitalWrite(Led,LOW);
      }

      if(mensaje == "Led esta en ON"){
        digitalWrite(Led,HIGH);
      }
  }
}

void setup(void)
{
  
  Serial.begin(115200);
  pinMode(Led, OUTPUT);
  
  WiFi.softAP("PuntoAcceso", "");
  Serial.println("softAP");
  Serial.println("");
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

void loop(void)
{
 websockets.loop();
}
