
// Bibliotecas
#include "painlessMesh.h"
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>


// Pines para la comunicación UART Serial2
#define RXD2 16
#define TXD2 17

// Nombre de la malla, contraseña y puerto
#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555


// Variables
int led;
int led_status;
int board_status;

String message = "";
bool messageReady = false;
bool message_received = false;


// Instancia/objeto del organizador de tareas
Scheduler userScheduler;

// Instancia/objeto de la malla
painlessMesh  mesh;


// Función enviarMensaje
void sendMessage();

// Tarear enviarMensaje
Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage()
{

DynamicJsonDocument doc(1024);
  doc["board"] = board_status;
  doc["pin"] = led;
  doc["status"] =  led_status;
  String msg ;
  serializeJson(doc, msg);
  mesh.sendBroadcast( msg );
  Serial.print("Difusión a la malla: - "); 
  Serial.println(msg);
}


// Función que recibe el mensaje
void receivedCallback( uint32_t from, String &msg ) {}

// Función que detecta nuevas conexiones/nodos
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> Detectada: Nueva conexión, nodeId = %u\n", nodeId);
}

// Función que detecta cambios en la conexiones/nodos
void changedConnectionCallback() {
  Serial.printf("Cambios en las conexiones\n");
}

// Función que sincroniza/ajusta la hora
void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Hora ajustada %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}

void loop()
{
   while (Serial2.available()) {
    message = Serial2.readString();
    Serial.println(message);
    messageReady = true;
    Serial.print("Mensaje recibido "); 
    Serial.println(message);
  }
  DynamicJsonDocument doc(1024);
  DeserializationError error = deserializeJson(doc, message);
  board_status = doc["board"];
  led = doc["pin"];
  led_status = doc["pin_status"];

  messageReady = false;

  mesh.update();
}
