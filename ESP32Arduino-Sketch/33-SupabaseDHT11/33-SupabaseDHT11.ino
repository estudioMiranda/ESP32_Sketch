#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

#define DHTPIN 25   
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "studiomiranda";
const char* password =  "88888888";

// Supabase
String url = "https://vommeepgegbaadwlmcns.supabase.co/rest/v1/ESP32";
String apikey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InZvbW1lZXBnZWdiYWFkd2xtY25zIiwicm9sZSI6ImFub24iLCJpYXQiOjE2NTc1NTU0NDUsImV4cCI6MTk3MzEzMTQ0NX0.COSffX_5xvZOJrqt7ddbwGV32KggfYK-TeZRTUyeC_c";

char objetoJson[128];

void setup() {
 
  Serial.begin(115200);
  
  dht.begin();
  
  WiFi.begin(ssid, password);
  Serial.print("Conectando...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("Conexión Wifi establecida, mi dirección IP es: ");
  Serial.println(WiFi.localIP());

}

void loop(){
  if ((WiFi.status()== WL_CONNECTED)) {

  float h = dht.readHumidity();
  float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
    Serial.println(F("¡Error en la lectura del sensor DHT!"));
    return;
  }

  Serial.println();
  Serial.print(F("Humedad: "));
  Serial.print(h);
  Serial.print(F("%  Temperatura: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.println();
    
    HTTPClient client;
    
    client.begin( url );
    client.addHeader("apikey", apikey);
    client.addHeader("Content-Type", "application/json");

    const size_t CAPACITY = JSON_OBJECT_SIZE(3);
    StaticJsonDocument<CAPACITY> doc;

    JsonObject obj= doc.to<JsonObject>();
    obj ["id"] = "1";
    obj ["temperatura"] = t;
    obj ["humedad"] = h;

    serializeJson (doc, objetoJson);
    Serial.println("Objeto json: ");
    Serial.print(objetoJson);
    
    int httpCode = client.PATCH(String(objetoJson));

      if (httpCode > 0) {
          Serial.println();
          Serial.printf("Código de respuesta: %d\t", httpCode);
      
          if (httpCode == HTTP_CODE_OK) {
            String payload = client.getString();
            Serial.println(payload);
          }
        }
        else {
          Serial.printf("Error de la solicitud, error: %s\n", client.errorToString(httpCode).c_str());
        }
    client.end();
    }
    else {
     Serial.println("Conexión perdida");
     }  
  delay (3000);
}
