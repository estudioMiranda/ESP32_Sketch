
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#define Led 2

char paginaweb[] PROGMEM = R"=====(

<!DOCTYPE html>
<html>
<body>

<center>

<h1>ESP32 HTML</h1>

<h3> LED </h3>

<button onclick="window.location = 'http://192.168.4.1/led/on'">On</button>
<button onclick="window.location = 'http://'+location.hostname+'/led/off'">Off</button>

</center>
</body>
</html>

)=====";


AsyncWebServer server(80); // server port 80

void notFound(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Página no encontrada");
}

void setup(void)
{
  
  Serial.begin(115200);
  pinMode(Led, OUTPUT);

  
  WiFi.softAP("PuntoAcceso");
  Serial.println("softAP: ");
  Serial.println(WiFi.softAPIP());
  
  server.on("/", [](AsyncWebServerRequest * request)
  { 
   
  request->send_P(200, "text/html", paginaweb);
  });

   server.on("/led/on", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    digitalWrite(Led, HIGH);
  request->send_P(200, "text/html", paginaweb);
  });

  server.on("/led/off", HTTP_GET, [](AsyncWebServerRequest * request)
  { 
    digitalWrite(Led, LOW);
  request->send_P(200, "text/html", paginaweb);
  });

  server.onNotFound(notFound);

  server.begin();  // Finalmente inciamos el servidor
}


void loop(void)
{
}
