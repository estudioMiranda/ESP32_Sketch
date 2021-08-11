
#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <Adafruit_Sensor.h> //DHT sensor library esp32
#include <DHT.h>

// Configura la ID de la placa de desarrollo
#define BOARD_ID 1

// Pin del sensor DHT
#define DHTPIN 4  

// Descomenta el tipo de sensor usado:
#define DHTTYPE    DHT11     
//#define DHTTYPE    DHT22    
//#define DHTTYPE    DHT21     

DHT dht(DHTPIN, DHTTYPE);

// Direción MAC del receptor  
uint8_t broadcastAddress[] = {0x08, 0x3A, 0xF2, 0x85, 0xE1, 0x68};

// Estructura del mensaje de datos DHT
typedef struct struct_message {
    int id;
    float temp;
    float hum;
    int readingId;
} struct_message;

// Mensaje con los datos 
struct_message myData;

unsigned long previousMillis = 0;   // Almacena los últimos datos publicados
const long interval = 10000;        // Intervalo de lectura del sensor DHT

unsigned int readingId = 0;

// Nombre de la conexión devuelve el canal
constexpr char WIFI_SSID[] = "studiomiranda";

int32_t getWiFiChannel(const char *ssid) {
  if (int32_t n = WiFi.scanNetworks()) {
      for (uint8_t i=0; i<n; i++) {
          if (!strcmp(ssid, WiFi.SSID(i).c_str())) {
              return WiFi.channel(i);
          }
      }
  }
  return 0;
}

float readDHTTemperature() {
  
  // Lectura de la temperatura en centígrados
  float t = dht.readTemperature();
  
  // Comprueba si las lecturas son válidas
  if (isnan(t)) {    
    Serial.println("¡A ocurrido un fallo en la lectura del sensor DHT!");
    return 0;
  }
  else {
    Serial.println(t);
    return t;
  }
}

float readDHTHumidity() {
  // Lectura del sensor de humedad
  float h = dht.readHumidity();
  if (isnan(h)) {
    Serial.println("¡A ocurrido un fallo en la lectura del sensor DHT!");
    return 0;
  }
  else {
    Serial.println(h);
    return h;
  }
}

// Notifica si los datos han sido recibidos
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nEstado del último paquete enviado:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Entregado" : "A ocurrido un error en la entrega");
}
 
void setup() {
  // Configuramos los baudios
  Serial.begin(115200);

  dht.begin();
 
  // Configuramos la conexión WiFi como ESTACIÓN
  WiFi.mode(WIFI_STA);

  // Obtenemos el canal
  int32_t channel = getWiFiChannel(WIFI_SSID);

  WiFi.printDiag(Serial); // Muestra el canal
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);
  WiFi.printDiag(Serial); // Verifica que el canal no ha cambiado

  // Inicia la conexión ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("A ocurrido un error al iniciar ESP-NOW");
    return;
  }

  // Registramos/nofificación los envíos de los paquetes
  esp_now_register_send_cb(OnDataSent);
  
  // Regitramos el emparejamiento
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.encrypt = false;
  
  // Añadir par (emparejar)        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("A ocurrido un error de emparejamiento");
    return;
  }
}
 
void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // Actualiza el tiempo de la nueva lectura publicada
    previousMillis = currentMillis;
    // Actualiza los Datos del sensor DHT de la placa de desarrollo 
    myData.id = BOARD_ID;
    myData.temp = readDHTTemperature();
    myData.hum = readDHTHumidity();
    myData.readingId = readingId++;
     
    // Envía el mensaje a la placa dirección MAC 
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    if (result == ESP_OK) {
      Serial.println("Datos enviados");
    }
    else {
      Serial.println("A ocurrido un error en el envío de datos");
    }
  }
}
