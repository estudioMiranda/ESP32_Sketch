//  Activa la base de datos Supabase como RealTime en:
//  Database => Replication => Source => botón "tables" => activa la tabla

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

#include "supabaseAPI.hpp"
 
#define RELE1PIN 19 
#define RELE2PIN 22 
#define RELE3PIN 21 

#define INTERRUPTOR1PIN 35
#define INTERRUPTOR2PIN 34
#define INTERRUPTOR3PIN 33


#define DHTPIN 23
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

bool state1;
bool state2;
bool state3;

float temp;
float hum;

const char* ssid = "studio";
const char* password =  "88888888";

char objetoJson[128];

int id = 1;

String payload; 

void setup() {
 
  Serial.begin(115200);

  //pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELE1PIN, OUTPUT);
  pinMode(RELE2PIN, OUTPUT);
  pinMode(RELE3PIN, OUTPUT);

  pinMode(INTERRUPTOR1PIN, INPUT);
  pinMode(INTERRUPTOR2PIN, INPUT);
  pinMode(INTERRUPTOR3PIN, INPUT);

  pinMode(DHTPIN, INPUT);

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

void loop() {
  if ((WiFi.status()== WL_CONNECTED)) {  

  temp = dht.readTemperature();
  hum = dht.readHumidity();
    if (isnan(temp) || isnan(hum)) {
    Serial.println(F("¡Error en la lectura del sensor DHT!"));
    return;
  }

	const size_t CAPACITY = JSON_OBJECT_SIZE(6);
    StaticJsonDocument<CAPACITY> doc;

    JsonObject obj= doc.to<JsonObject>();
    obj ["id"] = id;
    obj ["luz1"] = state1;
    obj ["luz2"] = state2;
    obj ["luz3"] = state3;
    obj ["temperatura"] = temp;
    obj ["humedad"] = hum;
    
    serializeJson (doc, objetoJson);
    Serial.println("Objeto json: ");
    Serial.println(objetoJson);

  UpdateById( 1, objetoJson);  // ACTUALIZAMOS LA BASE DE DATOS

  String datos = GetById(1);
  
  char documentoJson[500];
        datos.replace(" ", "");
        datos.replace("\n", "");
        datos.trim();
        datos.remove(0,1);
        datos.remove(payload.length()-1, 1);
        datos.toCharArray(documentoJson, 500);

        Serial.println(datos);
        
        StaticJsonDocument<200> documento;
        deserializeJson( documento, documentoJson);

        state1 = documento["luz1"];
        state2 = documento["luz2"];
        state3 = documento["luz3"];

        Serial.println(state1);
        Serial.println(state2);
        Serial.println(state3);
        
  if (digitalRead(INTERRUPTOR1PIN) == LOW) {
    state1 = !state1;
    digitalWrite(RELE1PIN, state1);
    } else {
    state1 = !state1;
    digitalWrite(RELE1PIN, state1);
    Serial.println("Estado del relé es: ");
    Serial.println(state1);
  }      
      
  if (state1 == true) {
    digitalWrite(RELE1PIN, HIGH);
    Serial.println("encendido1");
  } else {
    digitalWrite(RELE1PIN, LOW);
    Serial.println("apagado1");
  }
  
  if (state2 == true) {
    digitalWrite(RELE2PIN, HIGH);
    Serial.println("encendido2");
  } else {
    digitalWrite(RELE2PIN, LOW);
    Serial.println("apagado2");
  }

  if (state3 == true) {
    digitalWrite(RELE3PIN, HIGH);
    Serial.println("encendido3");
  } else {
    digitalWrite(RELE3PIN, LOW);
    Serial.println("apagado3");
  }
  
  }  
   else {
    Serial.println("Conexión perdida");
  } 
}
