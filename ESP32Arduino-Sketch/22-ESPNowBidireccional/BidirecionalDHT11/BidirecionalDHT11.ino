// Bibliotecas
#include <esp_now.h>
#include <WiFi.h>

// Bibliotecas sensorDHT
#include <Wire.h>
#include "DHT.h"

#define DHTPIN 4  

// Bibliotecas pantalla OLED SSD1306 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Resolución pantalla
#define SCREEN_WIDTH 128  // Pantalla OLED ancho (pixels)
#define SCREEN_HEIGHT 64  // Pantalla OLED alto (pixels)

// Pasamos el ancho y alto de la pantalla
// El tipo de conexión I2C pines SDA y SCL biblioteca "Wire"
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTTYPE DHT11   // DHT 11

// Initialize DHT sensorDHT
DHT sensorDHT(DHTPIN, DHTTYPE);


// REPLACE WITH THE MAC Address of your receiver 
uint8_t direccionMacEntrega[] = {0x24, 0x6f, 0x28, 0xab, 0xc0, 0x60};

// Variables de la lectura del sensorDHT
float temperatura;
float humedad;

// Variables de datos recibidos
float tempRecibida;
float humRecibida;
float presRecibida;

// Variable que confirma la entrega
String entregado;

// Estructura de los datos a enviar
typedef struct paquete {
    float temp;
    float hum;
    float pres;
} paquete;

// Guarda las lecturas del sensorDHT
paquete lecturasSensorDHT;

// Guarda las lecturas recibidas
paquete lecturasRecibidas;

// Notifica si los datos han sido entregados
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nEstado del último paquete enviado:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Entregado" : "Ha ocurrido un error en la entrega");
  // Este condicional muestra en la pantalla el texto
  if (status == 0){
    entregado = "Entregado";
  }
  else{
    entregado = "Ha ocurrido un error en la entrega";
  }
}

// Función que recibe los datos de la otra placa
void OnDataRecv(const uint8_t * mac, const uint8_t *datosRecibidos, int len) {
  memcpy(&lecturasRecibidas, datosRecibidos, sizeof(lecturasRecibidas));
  tempRecibida = lecturasRecibidas.temp;
  humRecibida = lecturasRecibidas.hum;
  presRecibida = lecturasRecibidas.pres;
  Serial.print("Bytes recibidos: ");
  Serial.println(len);
  Serial.print("temperatura: ");
  Serial.println(tempRecibida);
  Serial.print("Humedad: ");
  Serial.println(humRecibida);
}
 
void setup() {
  // Baudios monitor serie
  Serial.begin(115200);

  sensorDHT.begin();
  
   // Inicia la pantalla
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("No detecta pantalla"));
    for(;;);
  }
 
  // Configuración del WiFi modo Estación
  WiFi.mode(WIFI_STA);

  // Inicia la conexión ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Ha ocurrido un error al iniciar ESP-NOW");
    return;
  }

  // Creamos un registro de los datos enviados
  esp_now_register_send_cb(OnDataSent);
  
  // Configuramos el emparejamiento
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, direccionMacEntrega, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Realizamos el emparejamiento      
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Ha ocurrido un error de emparejamiento");
    return;
  }
  // Registramos la función de los datos recibidos
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  datosLecturaSensorDHT();
 
  // 
  lecturasSensorDHT.temp = temperatura;
  lecturasSensorDHT.hum = humedad;


  // Send message via ESP-NOW
  esp_err_t resultado = esp_now_send(direccionMacEntrega, (uint8_t *) &lecturasSensorDHT, sizeof(lecturasSensorDHT));
   
    if (resultado == ESP_OK) {
    Serial.println("Enviado");
  }
  else {
    Serial.println("Ha ocurrido un error en envío");
  }
//  actualizarPantalla();
  delay(10000);
}
void datosLecturaSensorDHT(){
  temperatura = sensorDHT.readTemperature();
  humedad = sensorDHT.readHumidity();
}

void actualizarPantalla(){
  // Display Readings on OLED Display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Lectura datos recibidos:");
  display.setCursor(0, 15);
  display.print("Temperatura: ");
  display.print(tempRecibida);
  display.cp437(true);
  display.write(248);
  display.print("C");
  display.setCursor(0, 25);
  display.print("Humedad: ");
  display.print(humRecibida);
  display.print("%");
  display.setCursor(0, 35);
  display.print("Pressure: ");
  display.print(incomingPres);
  display.print("hPa");
  display.setCursor(0, 56);
  display.print(success);
  display.display();
}
