// Bibliotecas
#include <esp_now.h>
#include <WiFi.h>

// Bibliotecas sensor BME280
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Bibliotecas pantalla OLED SSD1306 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Resolución pantalla
#define SCREEN_WIDTH 128  // Pantalla OLED ancho (pixels)
#define SCREEN_HEIGHT 64  // Pantalla OLED alto (pixels)

// Pasamos el ancho y alto de la pantalla
// El tipo de conexión I2C pines SDA y SCL biblioteca "Wire"
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Instancia/objeto del sensor
Adafruit_BME280 sensor;

// REPLACE WITH THE MAC Address of your receiver 
uint8_t direccionMacEntrega[] = {0x08, 0x3a, 0xf2, 0x85, 0xe1, 0x68};

// Variables de la lectura del sensor
float temperatura;
float humedad;
float presion;

// Variables de datos recibidos
float tempRecibida;
float humRecibida;

// Variable que confirma la entrega
String entregado;

// Estructura de los datos a enviar
typedef struct paquete {
    float temp;
    float hum;
    float pres;
} paquete;

// Guarda las lecturas del sensor
paquete lecturasSensor;

// Guarda las lecturas recibidas
paquete lecturasRecibidas;

// Notifica si los datos han sido entregados
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nEstado del último paquete enviado:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Entregado" : "Ha ocurrido un error en la entrega");
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
  Serial.print("Bytes recibidos: ");
  Serial.println(len);
  Serial.print("Temperatura: ");
  Serial.println(tempRecibida);
  Serial.print("humRecibida: ");
  Serial.println(humRecibida);
}
 
void setup() {
  // Baudios monitor serie
  Serial.begin(115200);

  // Inicia el sensor
  bool status = sensor.begin(0x76);  
  if (!status) {
    Serial.println("No detecta ningún sensor, revisa los cables!");
    while (1);
  }

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
  datosLecturaSensor();
 
  // 
  lecturasSensor.temp = temperatura;
  lecturasSensor.hum = humedad;
  lecturasSensor.pres = presion;

  // Send message via ESP-NOW
  esp_err_t resultado = esp_now_send(direccionMacEntrega, (uint8_t *) &lecturasSensor, sizeof(lecturasSensor));
   
    if (resultado == ESP_OK) {
    Serial.println("Enviado");
  }
  else {
    Serial.println("Ha ocurrido un error en envío");
  }
  actualizarPantalla();
  delay(10000);
}
void datosLecturaSensor(){
  temperatura = sensor.readTemperature();
  humedad = sensor.readHumidity();
  presion = (sensor.readPressure() / 100.0F);
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
  display.setCursor(0, 56);
  display.print(success);
  display.display();
}
