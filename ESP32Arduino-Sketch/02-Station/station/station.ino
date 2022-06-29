#include <WiFi.h>

void setup(void)
{
  Serial.begin(115200);
  Serial.println();
  
  // Pon aquí tu ssid y contraseña
  WiFi.begin("Museo", "sCientific0s"); 

  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Conectado, dirección IP: ");
  Serial.println(WiFi.localIP());
}

void loop(void)
{
}
