#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "studio";
const char* password =  "88888888";

char jsonOutput[128];

float temperatura = 22.50;
float humedad = 50;

void setup() {
 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando...");
  
  while (WiFi.status() != WL_CONNECTED) { //Check for the connection
    delay(500);
    Serial.print(".");
  }

  Serial.print("Conectado con éxito, mi IP es: ");
  Serial.println(WiFi.localIP());

}

void loop(){
  if ((WiFi.status()== WL_CONNECTED)) {
    
    HTTPClient client;
    
    client.begin("https://vommeepgegbaadwlmcns.supabase.co/rest/v1/ESP32");
    client.addHeader("apikey", "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InZvbW1lZXBnZWdiYWFkd2xtY25zIiwicm9sZSI6ImFub24iLCJpYXQiOjE2NTc1NTU0NDUsImV4cCI6MTk3MzEzMTQ0NX0.COSffX_5xvZOJrqt7ddbwGV32KggfYK-TeZRTUyeC_c");
    client.addHeader("Content-Type", "application/json");

    const size_t CAPACITY = JSON_OBJECT_SIZE(1);
    StaticJsonDocument<CAPACITY> doc;

    JsonObject object = doc.to<JsonObject>();
    object ["id"] = "6";

    serializeJson (doc, jsonOutput);
    
    int httpCode = client.POST(String(jsonOutput));

      if( httpCode > 0){     
        String payload = client.getString();
        Serial.println("\nStatuscode: " + String(httpCode));
        Serial.println(payload);

        client.end(); 
      }
        else {
          Serial.println("Error en la solicitud http");
    
        }
   }
    else {
     Serial.println("Conexión perdida");
    }
    
  delay (10000);
}

 
