/* 
  Modificado por StudioMiranda ArduinoES
 
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-web-server-websocket-sliders/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"
#include <Arduino_JSON.h>

// Cambia por el nombre de tu conexión y contraseña
const char* ssid = "studiomiranda";
const char* password = "88888888";

// Asignamos el servidor web al puerto 80
AsyncWebServer server(80);

// Creamos un objeto WebSocket
AsyncWebSocket ws("/ws");

// Configuramos los pines
const int ledPin1 = 2;  // Led integrado LED_BUILTIN
const int ledPin2 = 13;
const int ledPin3 = 14;

String message = "";

String sliderValue1 = "0"; // Valor del potenciómetro deslizador
String sliderValue2 = "0";
String sliderValue3 = "0";

int dutyCycle1; // Valor del potenciómetro deslizador
int dutyCycle2;
int dutyCycle3;

// Configuramos el PWM
const int freq = 5000;
const int ledChannel1 = 0;
const int ledChannel2 = 1;
const int ledChannel3 = 2;
const int resolution = 8;

// Variable JSON
JSONVar sliderValues;

// Función que genera un JSON de los valores de los potenciómetros
String getSliderValues(){
  sliderValues["sliderValue1"] = String(sliderValue1);
  sliderValues["sliderValue2"] = String(sliderValue2);
  sliderValues["sliderValue3"] = String(sliderValue3);

  String jsonString = JSON.stringify(sliderValues);
  Serial.println();
  Serial.print("Valor jsonString: ");
  Serial.println(jsonString);
  return jsonString;
  
}

// Inicia la memoria SPIFFS
void initFS() {
  if (!SPIFFS.begin()) {
    Serial.println("Ha ocurrido un error al iniciar SPIFFS");
  }
  else{
   Serial.println("SPIFFS memoria iniciada");
  }
}

// Inica la conexión WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void notifyClients(String sliderValues) {
  ws.textAll(sliderValues);
  Serial.print("Valor sliderValues: ");
  Serial.println(sliderValues);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    message = (char*)data;
      Serial.println();
      Serial.print("Valor mensaje: ");
      Serial.println(message);
    if (message.indexOf("1s") >= 0) {
      sliderValue1 = message.substring(2);
      dutyCycle1 = map(sliderValue1.toInt(), 0, 100, 0, 255);
      Serial.print("Valor dutyCycle1: ");
      Serial.println(dutyCycle1);
      Serial.print(getSliderValues());
      Serial.println();
      notifyClients(getSliderValues());
    }
    if (message.indexOf("2s") >= 0) {
      sliderValue2 = message.substring(2);
      dutyCycle2 = map(sliderValue2.toInt(), 0, 100, 0, 255);
      Serial.print("Valor dutyCycle2: ");
      Serial.println(dutyCycle2);
      Serial.print(getSliderValues());
      Serial.println();
      notifyClients(getSliderValues());
    }    
    if (message.indexOf("3s") >= 0) {
      sliderValue3 = message.substring(2);
      dutyCycle3 = map(sliderValue3.toInt(), 0, 100, 0, 255);
      Serial.print("Valor dutyCycle3: ");
      Serial.println(dutyCycle3);
      Serial.print(getSliderValues());
      Serial.println();
      notifyClients(getSliderValues());
    }
    if (strcmp((char*)data, "cargarValores") == 0) {
      notifyClients(getSliderValues());
    }
  }
}
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.printf("Cliente WebSocket #%u conectado desde %s\n", client->id(), client->remoteIP().toString().c_str());
      break;
    case WS_EVT_DISCONNECT:
      Serial.printf("Cliente WebSocket #%u desconectado\n", client->id());
      break;
    case WS_EVT_DATA:
      handleWebSocketMessage(arg, data, len);
      break;
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
      break;
  }
}

void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}


void setup() {
  Serial.begin(115200);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  initFS();
  initWiFi();

  // Configuramos la escritura analógica PWM
  ledcSetup(ledChannel1, freq, resolution);
  ledcSetup(ledChannel2, freq, resolution);
  ledcSetup(ledChannel3, freq, resolution);

  // Asigna el Pin al canal
  ledcAttachPin(ledPin1, ledChannel1);
  ledcAttachPin(ledPin2, ledChannel2);
  ledcAttachPin(ledPin3, ledChannel3);


  initWebSocket();
  
  // Inicia el servidor web 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", "text/html");
  });
  
  server.serveStatic("/", SPIFFS, "/");

  // Inicia el servidor
  server.begin();

}

void loop() {
  ledcWrite(ledChannel1, dutyCycle1);
  ledcWrite(ledChannel2, dutyCycle2);
  ledcWrite(ledChannel3, dutyCycle3);

  ws.cleanupClients();
}
