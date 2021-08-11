// Bibliotecas
#include "painlessMesh.h"
#include <ArduinoJson.h>

// Malla--> nombre, contraseña y puerto
#define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   "somethingSneaky"
#define   MESH_PORT       5555

int led;
int led_status = 0;
int board_status = 0;

Scheduler userScheduler;
painlessMesh  mesh;

// Función que recibe el paquete/mensaje
void receivedCallback( uint32_t from, String &msg )
{
  //Descompone el JSON
  String json;
  DynamicJsonDocument doc(1024);
  json = msg.c_str();
  DeserializationError error = deserializeJson(doc, json);
  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
  }
  board_status = doc["board"];
  led = doc["pin"];
  led_status = doc["status"];

  if (board_status == 1){
  digitalWrite(led, led_status);
  }
}
void newConnectionCallback(uint32_t nodeId) {
  Serial.printf("--> Detectada: Nueva conexión, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Cambio en las conexiones\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Hora ajustada %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void setup() {
  Serial.begin(115200);
  
  pinMode(2, OUTPUT);
  digitalWrite(2,LOW);

  mesh.setDebugMsgTypes( ERROR | STARTUP );
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback); 
}

void loop() {
  mesh.update();
}
