
#include <esp_now.h>
#include <WiFi.h>

// Ejemplo de variables que se pueden recibir
typedef struct mensaje {
    char a[32];
    int b;
    float c;
    bool d;
};

// La struct_message la guardamos en myData
mensaje datos;

// Función que obtiene el paquete/mensaje/datos 
void OnDataRecv(const uint8_t * mac, const uint8_t *datosRecibidos, int len) {
  memcpy(&datos, datosRecibidos, sizeof(datos));
  Serial.print("Bytes recibidos: ");
  Serial.println(len);
  Serial.print("Texto: ");
  Serial.println(datos.a);
  Serial.print("Enteros: ");
  Serial.println(datos.b);
  Serial.print("Decimales: ");
  Serial.println(datos.c);
  Serial.print("Booleanos: ");
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
