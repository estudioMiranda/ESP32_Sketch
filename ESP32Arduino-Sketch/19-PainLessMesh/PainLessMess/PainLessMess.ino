/* 
 ArduinoES por https://twitter.com/studiomiranda
  This is a simple example that uses the painlessMesh library: https://github.com/gmag11/painlessMesh/blob/master/examples/basic/basic.ino
*/

// Instala la biblioteca TaskScheduler (dependencia)
#include "painlessMesh.h"


#define   MESH_PREFIX     "ArduinoES"     // Nombre de la MALLA, igual para todos
#define   MESH_PASSWORD   "studioMiranda" // Contraseña, igual para todos
#define   MESH_PORT       5555            // Puerto, valor predeterminado


Scheduler userScheduler; // Programa/organiza las tareas

// Crea una instancia/objeto MALLA
painlessMesh  mesh;

// Función enviarMensaje
void sendMessage();

// TareaEnviarMensaje
Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );
  
  // Función enviarMensaje
  void sendMessage() {
    String msg = "Hola desde ArduinoES";
    msg += mesh.getNodeId();
    mesh.sendBroadcast( msg );
    taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
  }

// Función recibirMensaje
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Commienza aquí: Recibida desde %u msg=%s\n", from, msg.c_str());
}

// Función nuevaConexión, muestra el ID del nuevo nodo
void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> Comienza aqui: Nueva conexión, Id del nodo = %u\n", nodeId);
}

// Función cambioConexiones
void changedConnectionCallback() {
  Serial.printf("Cambios en las conexiones\n");
}

// Función ajustarTiempo
void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Sincronización de hora %u.compensación de = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);
  
// Función depuración
//mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE );
  mesh.setDebugMsgTypes( ERROR | STARTUP );  // Antes de init()

// Funciones
  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();
}

void loop() {
  mesh.update();
}
