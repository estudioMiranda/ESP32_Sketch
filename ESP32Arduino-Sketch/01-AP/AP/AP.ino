
#include <WiFi.h>

void setup(void)
{
  Serial.begin(115200);
  
  WiFi.softAP("PuntoAcceso", "contrasena");
  Serial.println("soft Access Point: ");
  Serial.println(WiFi.softAPIP());
  
}

void loop(void)
{
}
