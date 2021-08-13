
#include <esp_now.h>
#include <WiFi.h>

// Función que obtiene el paquete/mensaje/datos 
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

  char* datos = (char*) incomingData;
  String mensaje = String(datos);
  Serial.println(mensaje);
  
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
