
#include <esp_now.h>
#include <WiFi.h>

// Ejemplo de variables que se pueden recibir
typedef struct struct_message {
    char a[32];
    int b;
    float c;
    bool d;
} struct_message;

// La struct_message la guardamos en myData
struct_message myData;

// Función que obtiene el paquete/mensaje/datos 
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Char: ");
  Serial.println(myData.a);
  Serial.print("Int: ");
  Serial.println(myData.b);
  Serial.print("Float: ");
  Serial.println(myData.c);
  Serial.print("Bool: ");
  Serial.println(myData.d);
  Serial.println();
}
 
void setup() {
  // Configuramos los baudios
  Serial.begin(115200);
  
   // Configuración WiFi modo ESTACIÓN
  WiFi.mode(WIFI_STA);

  // Inicio comunicación ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("A ocurrido un error al iniciar ESP-NOW");
    return;
  }
  
  // Creamos un registro de estado de paquetes recibidos
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {

}
