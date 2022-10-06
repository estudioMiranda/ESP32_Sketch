#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

const char* ssid = "tu conexión a internet";
const char* password =  "tu contraseña";

// Supabase
String url = "https://mgqasbkqqquvixgxsfgo.supabase.co/rest/v1/DHT";
String apikey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Im1ncWFzYmtxcXF1dml4Z3hzZmdvIiwicm9sZSI6ImFub24iLCJpYXQiOjE2NjM5NTA0NjUsImV4cCI6MTk3OTUyNjQ2NX0.IOpbZhjdAJzMzpuhbT-lbkYmlprOHEbQMNenJHxwElk";

int id = 1;

void setup() {
 
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Conectando...");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.print("Conexión establecida, la IP local es: ");
  Serial.println(WiFi.localIP());

}

void loop(){
  
  if ((WiFi.status()== WL_CONNECTED)) {
    
    HTTPClient http;
    
    http.begin( url + "?id=eq." + String(id));
    http.addHeader("apikey", apikey);
    
    int httpCode = http.sendRequest("DELETE");

    id = id + 1;

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
