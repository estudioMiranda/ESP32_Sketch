
#include <esp_now.h>
#include <WiFi.h>

// Intruduce la MAC de la placa que recibe los datos
uint8_t macReceptora[] = {0x84, 0x0d, 0x8e, 0x37, 0x12, 0xb4};//84:0D:8E:37:12:B4

// Ejemplo de variables que se pueden enviar
typedef struct mensaje {
  char a[32];
  int b;
  float c;
  bool d;
} mensaje;

// El mensaje la guardamos en "datos"
mensaje datos;

// Notifica si los datos son recibidos
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nEstado del último paquete enviado:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Entregado" : "Ha ocurrido un fallo en la entrega");
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
  
  // Guardamos datos en la estructura para el emparejamiento con el receptor
  esp_now_peer_info_t datosEmparejamiento;
  memcpy ( datosEmparejamiento.peer_addr, macReceptora, 6 );
  datosEmparejamiento.channel = 0;  
  datosEmparejamiento.encrypt = false;
  
  // Añadimos el receptor      
  if (esp_now_add_peer(&datosEmparejamiento) != ESP_OK){
    Serial.println("Fallo de emparejamiento");
    return;
  }
}
 
void loop() {
  // Valores de los datos a enviar
  strcpy(datos.a, "Esto es un texto");
  datos.b = random(1,20);
  datos.c = 1.2;
  datos.d = false;
  
  // Envío de mensaje por ESP-NOW
  esp_err_t resultado = esp_now_send(macReceptora, (uint8_t *) &datos, sizeof(datos));
   
  if (resultado == ESP_OK) {
    Serial.println("Enviado correctamente");
  }
  else {
    Serial.println("Ha ocurrrido un error en el envío de datos");
  }
  delay(2000);
}
