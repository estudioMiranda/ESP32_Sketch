//  Activa la base de datos Supabase como RealTime en:
//  Database => Replication => Source => botón "tables" => activa la tabla

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

#include "supabaseAPI.hpp"

#define DHTPIN 25   
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "studio";
const char* password =  "88888888";

const int led = 2; // Led integrado (LED_BUILTIN)

// Propiedades (parámetros) PWM (modulación con pulsos)
const int freq = 5000;    // Hz => ciclos x segundo
const int ledChannel = 0; // pin
const int resolution = 8; //  8 bits  => rango de 0 a  255
                          // 10 bits  => rango de 0 a 1023

char objetoJson[128];

int id = 1; 
String payload; 

void setup() {
 
  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  
  ledcSetup(ledChannel, freq, resolution); // Añadimos los parámetros a la función
  ledcAttachPin(led, ledChannel);   

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

  float temperatura = dht.readTemperature();
  int humedad = dht.readHumidity();

    if (isnan(humedad) || isnan(temperatura)) {
    Serial.println(F("¡Error en la lectura del sensor DHT!"));
    return;
  }

  // Serial.println();
  // Serial.print(F("Humedad: "));
  // Serial.print(humedad);
  // Serial.print(F("%  Temperatura: "));
  // Serial.print(temperatura);
  // Serial.print(F("°C "));
  // Serial.println();

	const size_t CAPACITY = JSON_OBJECT_SIZE(3);
    StaticJsonDocument<CAPACITY> doc;

    JsonObject obj= doc.to<JsonObject>();
    obj ["id"] = id;
    obj ["temperatura"] = temperatura;
    obj ["humedad"] = humedad;

    serializeJson (doc, objetoJson);
    Serial.println("Objeto json: ");
    Serial.print(objetoJson);

  //UpdateById( 1, objetoJson);

  
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

        int encender = documento["encendido"];
        int pot = documento["potenciometro"];


  if (encender == 0) {
    ledcWrite(ledChannel, pot);
    Serial.println("encendido");
  } else {
    ledcWrite(ledChannel, 0);
    Serial.println("apagado");
  }
  
  Serial.print("Datos de la función: ");
  Serial.println(datos);
  Serial.print("Dato encender: ");
  Serial.println(encender);
  Serial.print("Dato potenciometro: ");
  Serial.println(pot);
  }  
    else {
      Serial.println("Conexión perdida");
    } 
}
