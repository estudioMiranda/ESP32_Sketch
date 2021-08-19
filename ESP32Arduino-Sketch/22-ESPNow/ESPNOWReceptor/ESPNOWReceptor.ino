
#include <esp_now.h>
#include <WiFi.h>

// Ejemplo de variables que se pueden recibir
typedef struct paquete {
    char a[32];
    int b;
    float c;
    bool d;
} paquete;

// La struct_message la guardamos en myData
paquete datos;

// Función que obtiene el paquete/paquete/datos 
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&datos, incomingData, sizeof(datos));
  Serial.print("Bytes recibidos: ");
  Serial.println(len);
  Serial.print("Dato tipo char: ");
  Serial.println(datos.a);
  Serial.print("Dato tipo int: ");
  Serial.println(datos.b);
  Serial.print("Dato tipo float: ");
  Serial.println(datos.c);
  Serial.print("Dato tipo bool: ");
  Serial.println(datos.d);
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
