
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

#define Led 2

AsyncWebServer server(80);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Página no encontrada");
}

void setup(void)
{
  
  Serial.begin(115200);
  pinMode(Led, OUTPUT);

  
  WiFi.softAP("PuntoAcceso", "");
  Serial.println("softAP");
  Serial.println("");
  Serial.println(WiFi.softAPIP());

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("A ocurrido un error al montando SPIFFS");
    return;
  }
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request){
    request->send(SPIFFS, "/index.html", "text/html");
  });

   server.on("/led/on", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    digitalWrite(Led, HIGH);
  request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/led/off", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    digitalWrite(Led, LOW);
  request->send(SPIFFS, "/index.html", "text/html");
  });

  server.onNotFound(notFound);

  server.begin();  // Finalmente inciamos el servidor
}


void loop(void)
{
}
