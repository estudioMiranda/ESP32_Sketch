#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "tu conexión a internet";
const char* password =  "tu contraseña";

// Supabase
String url = "https://mgqasbkqqquvixgxsfgo.supabase.co/rest/v1/DHT";
String apikey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Im1ncWFzYmtxcXF1dml4Z3hzZmdvIiwicm9sZSI6ImFub24iLCJpYXQiOjE2NjM5NTA0NjUsImV4cCI6MTk3OTUyNjQ2NX0.IOpbZhjdAJzMzpuhbT-lbkYmlprOHEbQMNenJHxwElk";

char salidaJson[128];

int id = 1;
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

  Serial.print("Conexión establecida, la IP local del ESP32 es: ");
  Serial.println(WiFi.localIP());

}

void loop(){
  
  if ((WiFi.status()== WL_CONNECTED)) {
    
    HTTPClient http;
    
    http.begin(url + "?id=eq." + String(id) + "&temperatura,humedad");
    http.addHeader("apikey", apikey);
    http.addHeader("Content-Type", "application/json");

    const size_t capacity = JSON_OBJECT_SIZE(2);
    StaticJsonDocument<capacity> doc;

    JsonObject obj= doc.to<JsonObject>();
    obj ["temperatura"] = temperatura;
    obj ["humedad"] = humedad;

    temperatura = temperatura + 1;
    humedad = humedad + 1;

    serializeJson (doc, salidaJson);
    Serial.println(salidaJson);
    
    int httpCode = http.PATCH(String(salidaJson));

      if( httpCode > 0){     
        Serial.println("\nCódigo de estado: " + String(httpCode));

        http.end(); 
      }
        else {
          Serial.println("Ha ocurrido un error en la solicitud http");
    
        }
   }
    else {
     Serial.println("Sin conexión WiFi");
    }
    
  delay (10000);
}
