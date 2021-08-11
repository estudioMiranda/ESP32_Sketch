
// Bibliotecas
#include <esp_now.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <WiFiClient.h>

// Pines para la comunicación UART Serial2
#define RXD2 16
#define TXD2 17

// Direción MAC del receptor  
uint8_t broadcastAddress[] = {0x08, 0x3A, 0xF2, 0x85, 0xE1, 0x68};

String message = "";
bool messageReady = false;
bool message_received = false;

// Notifica si los datos han sido recibidos
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nEstado del último paquete enviado:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Entregado" : "A ocurrido un error en la entrega");
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
   
  WiFi.mode(WIFI_STA);
    // Inicia la conexión ESP-NOW
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

void loop()
{
   while (Serial2.available()) {
    message = Serial2.readString();
    messageReady = true;
    Serial.print("Mensaje recibido "); 
    Serial.println(message);
  }
//  DynamicJsonDocument doc(1024);
//  DeserializationError error = deserializeJson(doc, message);
//  board_status = doc["board"];
//  led = doc["pin"];
//  led_status = doc["pin_status"];

  messageReady = false;

    // Envío de mensaje por ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &message, sizeof(message));
   
  if (result == ESP_OK) {
    Serial.println("Enviado correctamente");
    Serial.print(message);
  }
  else {
    Serial.println("A ocurrrido un error en el envío de datos");
  }
  
  delay(2000);
}
