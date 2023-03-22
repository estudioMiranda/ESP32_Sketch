//  Activa la base de datos Supabase como RealTime en:
//  Database => Replication => Source => botón "tables" => activa la tabla

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "DHT.h"
 
#include "supabaseAPI.hpp"
 
#define RELE1PIN 0
#define RELE2PIN 22 
#define RELE3PIN 21 

#define INTERRUPTOR1PIN 2  // Boton
#define INTERRUPTOR2PIN 34
#define INTERRUPTOR3PIN 33

#define DHTPIN 23
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

bool estado_interruptor1_inicial = 0;
bool estado_interruptor1_anterior = 0;
bool estadoInterruptor2 = 0;
bool estadoInterruptor3 = 0;

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

void baseDatos () {
  String datos = GetById(1);    // OBTENEMOS LOS DATOS DE LA BASE DE DATOS
  
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
}

void actualizar () {
    const size_t CAPACITY = JSON_OBJECT_SIZE(6);  // Asignamos un tamaño al objeto Json
    StaticJsonDocument<CAPACITY> doc;           // Creamos el documento Json

    JsonObject obj= doc.to<JsonObject>();       // Introducimos los objetos al doc Json
    obj ["id"] = id;
    obj ["luz1"] = state1;
    obj ["luz2"] = state2;
    obj ["luz3"] = state3;
    obj ["temperatura"] = temp;
    obj ["humedad"] = hum;
    
    serializeJson (doc, objetoJson);    // Serializamos el "doc" a variable "char objetoJson"
    Serial.println("Objeto json: ");
    Serial.println(objetoJson);

  UpdateById( 1, objetoJson);   // ACTUALIZAMOS LA BASE DE DATOS
}
 
void setup() {
 
  Serial.begin(115200);

  //pinMode(LED_BUILTIN, OUTPUT);
  pinMode(RELE1PIN, OUTPUT);
  pinMode(RELE2PIN, OUTPUT);
  pinMode(RELE3PIN, OUTPUT);

  pinMode(INTERRUPTOR1PIN, INPUT_PULLUP);
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
  
  if ((WiFi.status()== WL_CONNECTED)) {   // Condición si hay conexion WiFi establecida
  temp = dht.readTemperature();           // Obtenemos la temperatura
  hum = dht.readHumidity();               // Obtenemos la humedad
    if (isnan(temp) || isnan(hum)) {      // Condición si no es un número el valor
    Serial.println(F("¡Error en la lectura del sensor DHT!"));  // Enviamos un error
    return;                               // No devuelve nada
  }

    estado_interruptor1_inicial = digitalRead(INTERRUPTOR1PIN); 
  
  if ((estado_interruptor1_inicial == HIGH) && (estado_interruptor1_anterior == LOW) || (estado_interruptor1_inicial ==LOW) && (estado_interruptor1_anterior == HIGH)){
    state1 = !state1;
    actualizar ();
    delay(10);
    }
    estado_interruptor1_anterior = estado_interruptor1_inicial;

    if (state1 == 1)  {
    digitalWrite(RELE1PIN, HIGH);
    Serial.println("ENCENDIDO");
    baseDatos ();
    } else {
    digitalWrite(RELE1PIN, LOW);
    Serial.println("APAGADO");
    baseDatos ();
    } 
  } 
    else {
    Serial.println("Conexión perdida");
  }
}
