
#include <esp_now.h>
#include <WiFi.h>

// Intruduce la MAC de la placa que recibe los datos
uint8_t broadcastAddress[] = {0x08, 0x3A, 0xF2, 0x85, 0xE1, 0x68};


String saludo = "Hola Mundo";

 
// Notifica si los datos son recibidos
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nEstado del último paquete enviado:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Entregado" : "Fallo en la entrega");
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
  // Creamos un registro de estado de paquetes transmitidos
  esp_now_register_send_cb(OnDataSent);
  
  // Registramos el emparejamiento con el receptor
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Añadimos el receptor      
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Fallo de emparejamiento");
    return;
  }
}
 
void loop() {
  
  // Envío de mensaje por ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) saludo.c_str(), sizeof(saludo));
  Serial.print (saludo.c_str());
   
  if (result == ESP_OK) {
    Serial.println("Enviado correctamente");
  }
  else {
    Serial.println("A ocurrrido un error en el envío de datos");
  }
  delay(2000);
  
}
