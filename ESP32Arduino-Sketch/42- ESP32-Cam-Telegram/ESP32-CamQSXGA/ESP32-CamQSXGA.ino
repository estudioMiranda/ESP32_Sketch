/*****************
ArduinoES
Por Carlos Miranda  
*****************/

// ------------------------------------------
// Bibliotecas incluidas con la Tarjeta ESP32
// ------------------------------------------

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp_camera.h"

// -----------------------
// Bibliotecas Adicionales
// -----------------------

#include "UniversalTelegramBot.h"
#include <ArduinoJson.h>

// -------------------
// Bibliotecas M5stack
// -------------------
//#include "battery.h"

// -----------------------------------
// Selecciona la tarjeta de desarrollo
// -----------------------------------

//#define CAMERA_MODEL_WROVER_KIT
#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
//#define CAMERA_MODEL_AI_THINKER

// --------------------------
// Configura tu conexión WiFi
// --------------------------

char ssid[] = "salvador";        // Credenciales
char password[] = "88888888"; 

// -----------------------------------
// Configura tu cuenta de Telegram BOT
// -----------------------------------

#define BOTtoken "6025685312:AAHJhZwGU82yaI_xB4b1pMcyQ7I66hJjC8o"
#define chat_id "495285388"

// -------------------------------------
// Incluye los archivos de configuración
// -------------------------------------

#include "camera_pins.h"
#include "camera_code.h"

// -------------
// Pin del flash 
// -------------

#define BUTTON 15

// ---------------------------------------------------------
// Instanciamos un CLIENTE de la Blibioteca WifiClientSecure
// Instanciamos un BOT de la Biblioteca UniversalTelegramBot
// ---------------------------------------------------------

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// ---------
// Variables 
// ---------

camera_fb_t * fb = NULL; // Buffer del fotograma de la cámara

bool isMoreDataAvailable(); // Si hay mas datos disponibles de camera_fb_t

// --------------------------------------
// Envía los datos en bloques de 512 byte 
// --------------------------------------

int currentByte;    // Byte actual
uint8_t* fb_buffer; // Buffer
size_t fb_length;   // Tamaño total de los datos

bool isMoreDataAvailable() {        // Devueve true / false
  return (fb_length - currentByte); // Resta del tamaño total los actual Byte
}

uint8_t getNextByte() {      // Obtiene los nuevos bytes
  currentByte++;             // Incrementa el actual Byte
  return (fb_buffer[currentByte - 1]); // Suguiente bloque 512 bytes
}

bool dataAvailable = false;

void enviarFotoTelegram(){
  
    fb = NULL;
  
    sensor_t * s = esp_camera_sensor_get();
    s->set_framesize(s, FRAMESIZE_QSXGA);

    fb = esp_camera_fb_get();
    esp_camera_fb_return(fb);
    fb = esp_camera_fb_get();
    esp_camera_fb_return(fb);
  
    Serial.println("\n\n\nEnviando QSXGA");
  
    // Obteniendo una captura
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Error de captura");
      bot.sendMessage(chat_id, "Error de captura", "");
      return;
    }
  
    currentByte = 0;
    fb_length = fb->len;
    fb_buffer = fb->buf;
  
    Serial.println("\n>>>>> Enviando bloques de 512 byte, bytes=  " + String(fb_length));
  
    bot.sendPhotoByBinary(chat_id, "image/jpeg", fb_length,
                          isMoreDataAvailable, getNextByte,
                          nullptr, nullptr);
    
    esp_camera_fb_return(fb);
  } 

void setup() {
  
  Serial.begin(115200);

//  bat_init();

  if (!setupCamera()) {
    Serial.println("¡Error de configuración en la cámara!");
    while (true) {
      delay(100);
    }
  }

  Serial.print("Conectando a la red Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("Conexión WiFi establecida");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  client.setInsecure();
}


void loop() {

  bool foto = digitalRead(BUTTON);

  if (foto == false) {
  enviarFotoTelegram();
  }

  enviarFotoTelegram();
  delay(5000);
  
}
