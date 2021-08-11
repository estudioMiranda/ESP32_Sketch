
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>

const int led = 2; // Led integrado (LED_BUILTIN)

// Propiedades (parámetros) PWM (modulación con pulsos)
const int freq = 5000;    // Hz => ciclos x segundo
const int ledChannel = 0; // pin
const int resolution = 8; //  8 bits  => rango de 0 a  255
                          // 10 bits  => rango de 0 a 1023
AsyncWebServer server(80); 
WebSocketsServer websockets(81);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Página no encontrada");
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
      }
      break;
    case WStype_TEXT:
      Serial.printf("[%u] Texto: %s\n", num, payload);
      String mensaje = String((char*)( payload));
      Serial.println(mensaje);
      
      DynamicJsonDocument doc(200); // documento (capacidad)
      DeserializationError error = deserializeJson(doc, mensaje);
      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
        }
        
      int dutyCycle = doc["Led"];
      ledcWrite(ledChannel, dutyCycle); // Realizamos la escritura analógica
      }
}

void setup(void)
{
  pinMode(led, OUTPUT);
  ledcSetup(ledChannel, freq, resolution); // Añadimos los parámetros a la función
  ledcAttachPin( led, ledChannel);         // Asignamos el pin al canal
  
  Serial.begin(115200);
  
 // Pon aquí tu ssid y contraseña
  WiFi.begin("studiomiranda", "88888888");
  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Conectado, dirección IP: ");
  Serial.println(WiFi.localIP());
  
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

void loop(void) {
  websockets.loop();
}
