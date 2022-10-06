#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "studio";
const char* password =  "88888888";

// Supabase
String url = "https://mgqasbkqqquvixgxsfgo.supabase.co/rest/v1/DHT";
String apikey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Im1ncWFzYmtxcXF1dml4Z3hzZmdvIiwicm9sZSI6ImFub24iLCJpYXQiOjE2NjM5NTA0NjUsImV4cCI6MTk3OTUyNjQ2NX0.IOpbZhjdAJzMzpuhbT-lbkYmlprOHEbQMNenJHxwElk";


char jsonOutput[128];

int id = 3;
float temperatura = 22.50;
float humedad = 50;

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
    
    client.begin(url);
    client.addHeader("apikey", apikey);
    client.addHeader("Content-Type", "application/json");

    const size_t CAPACITY = JSON_OBJECT_SIZE(3);
    StaticJsonDocument<CAPACITY> doc;

    JsonObject object = doc.to<JsonObject>();
    object ["id"] = id;
    object ["temperatura"] = temperatura;
    object ["humedad"] = humedad;
    
    id = id + 1;
    
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

 
