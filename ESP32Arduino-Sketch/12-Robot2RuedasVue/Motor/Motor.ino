
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <Motor.h>

Motor motor = Motor(27,26,25,33);
const int ledPin = 2;
int freq = 5000;
int ledChannel = 0;
int resolution = 8;


AsyncWebServer server(80); 
WebSocketsServer websockets(81);

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Página no encontrada");
}void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

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

      
     DynamicJsonDocument doc(200);
  
    DeserializationError error = deserializeJson(doc, mensaje);
   
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }
  
  int direccion = doc["Motor"];
  int dutyCycle = doc["Velocidad"];

  Serial.print("Dato dirección: ");
  Serial.print(direccion);
  Serial.println();
  Serial.print("Dato velocidad: ");
  Serial.print(dutyCycle);
  
  ledcWrite(ledChannel, dutyCycle);

  switch (direccion) {
    case 1 :
      motor.Avanzar();
      break;
    case 2 :
      motor.Retroceder();
      break;
    case 3 :
      motor.GirarIzquierda();
      break;
    case 4 :
      motor.GirarDerecha();
      break;
    case 0 :
      motor.Parar();
      break;
    }
  }
}

void setup(void)
{
  pinMode(ledPin, OUTPUT);
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(ledPin, ledChannel);
   
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

void loop(void)
{
 websockets.loop();
}
