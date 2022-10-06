#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "tu conexión a internet";
const char* password =  "tu contraseña";

// Supabase tu URL y API key
String url = "https://mgqasbkqqquvixgxsfgo.supabase.co/rest/v1/DHT";
String apikey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Im1ncWFzYmtxcXF1dml4Z3hzZmdvIiwicm9sZSI6ImFub24iLCJpYXQiOjE2NjM5NTA0NjUsImV4cCI6MTk3OTUyNjQ2NX0.IOpbZhjdAJzMzpuhbT-lbkYmlprOHEbQMNenJHxwElk";

void setup() {
 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("Conectado con éxito, mi IP es: ");
  Serial.println(WiFi.localIP());
}

void loop(){
  if ((WiFi.status()== WL_CONNECTED)) {
    
    HTTPClient client;
    
    client.begin( url );
    client.addHeader("apikey", apikey);
    int httpCode = client.GET();

      if( httpCode > 0){     
        String payload = client.getString();
        Serial.println("\nCódigo de estado: " + String(httpCode));
        Serial.println(payload);

        char documentoJson[500];
        payload.replace(" ", "");
        payload.replace("\n", "");
        payload.trim();
        payload.remove(0,1);
        payload.remove(payload.length()-1, 1);
        payload.toCharArray(documentoJson, 500);

        Serial.println(payload);
        
        StaticJsonDocument<200> doc;
        deserializeJson( doc, documentoJson);

        float t = doc["temperatura"];
        int h = doc["humedad"];
        
        Serial.println("Temperatura: " + String(t) + ", " + "humedad: " + String(h) + "\n");
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

 
