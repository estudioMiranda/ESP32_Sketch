#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "studio";
const char* password =  "88888888";

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
    
    client.begin("https://vommeepgegbaadwlmcns.supabase.co/rest/v1/ESP32?select=*");
    client.addHeader("apikey", "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InZvbW1lZXBnZWdiYWFkd2xtY25zIiwicm9sZSI6ImFub24iLCJpYXQiOjE2NTc1NTU0NDUsImV4cCI6MTk3MzEzMTQ0NX0.COSffX_5xvZOJrqt7ddbwGV32KggfYK-TeZRTUyeC_c");
    int httpCode = client.GET();

      if( httpCode > 0){     
        String payload = client.getString();
        Serial.println("\nStatuscode: " + String(httpCode));
        Serial.println(payload);

        char json[500];
        payload.replace(" ", "");
        payload.replace("\n", "");
        payload.remove(0, 1);
        payload.toCharArray(json, 500);

        StaticJsonDocument<200> doc;
        deserializeJson( doc, json);

        int t = doc["temperatura"];
        int h = doc["humedad"];
        Serial.println(String(t) + "-" + String(h) + "\n");
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

 
