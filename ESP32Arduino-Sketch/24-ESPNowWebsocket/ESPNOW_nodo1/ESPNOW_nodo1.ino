// Bibliotecas
#include <esp_now.h>
#include <WiFi.h>
#include <ArduinoJson.h>

int led;
int led_status = 0;
int board_status = 0;
String message;

// Función que obtiene el paquete/mensaje/datos 
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&message, incomingData, sizeof(message));
  Serial.print("Bytes recibidos: ");
  Serial.println(len);
  Serial.print("Paquete: ");
  Serial.println(message);
  String json;
  DynamicJsonDocument doc(1024);
  json = message;
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


void setup() {
  Serial.begin(115200);
  
  pinMode(2, OUTPUT);
  digitalWrite(2,LOW);
  
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
