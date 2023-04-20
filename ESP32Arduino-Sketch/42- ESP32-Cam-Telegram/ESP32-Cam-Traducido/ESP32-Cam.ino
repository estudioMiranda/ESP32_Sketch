/*******************************************************************
ArduinoES
Por Carlos Miranda  
*******************************************************************/

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

// -----------------------------------
// Selecciona la tarjeta de desarrollo
// -----------------------------------

//#define CAMERA_MODEL_WROVER_KIT
//#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
#define CAMERA_MODEL_AI_THINKER

// --------------------------
// Configura tu conexión WiFi
// --------------------------

char ssid[] = "esp32";        // Credenciales
char password[] = "88888888"; 

// -----------------------------------
// Configura tu cuenta de Telegram BOT
// -----------------------------------

#define BOTtoken "6025685312:AAHJhZwGU82yaI_xB4b1pMcyQ7I66hJjC8o"

// -------------------------------------
// Incluye los archivos de configuración
// -------------------------------------

#include "camera_pins.h"
#include "camera_code.h"

// -------------
// Pin del flash 
// -------------

#define FLASH_LED_PIN 4

// ---------------------------------------------------------
// Instanciamos un CLIENTE de la Blibioteca WifiClientSecure
// Instanciamos un BOT de la Biblioteca UniversalTelegramBot
// ---------------------------------------------------------

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// ---------
// Variables 
// ---------

int Bot_mtbs = 1000; // Tiempo de espera para los nuevos mensajes
long Bot_lasttime;   // Referencia de la último tiempo espera

bool flashState = LOW; // Estado del flash (apagado)

camera_fb_t * fb = NULL; // Buffer del fotograma de la cámara

bool isMoreDataAvailable(); // Si hay mas datos disponibles de camera_fb_t

// --------------------------------------
// Envía los datos en bloques de 512 byte 
// --------------------------------------

int currentByte; // Byte actual
uint8_t* fb_buffer; // Buffer
size_t fb_length; // Tamaño total de los datos

bool isMoreDataAvailable() {
  return (fb_length - currentByte); // Resta del tamaño total los actual Byte
}

uint8_t getNextByte() {
  currentByte++; // Incrementa el actual Byte
  return (fb_buffer[currentByte - 1]); 
}

bool dataAvailable = false;

// ---------------------------
//Envía la foto como un bloque 
// ---------------------------

bool isMoreDataAvailableXXX() {
  if (dataAvailable)
  {
    dataAvailable = false;
    return true;
  } else {
    return false;
  }
}

byte* getNextBuffer() {
  if (fb) {
    return fb->buf;
  } else {
    return nullptr;
  }
}

int getNextBufferLen() {
  if (fb) {
    return fb->len;
  } else {
    return 0;
  }
}

// ----------------------------
// Función para nuevos mensajes
// ----------------------------

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    Serial.printf("\nTienes un mensaje %s\n", text);

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    String hi = "hola ... \nObteniendo: ";
    hi += text;
    bot.sendMessage(chat_id, hi, "Markdown");

    if (text == "/flash") {       // Si recibimos el texto de /flash
      flashState = !flashState;   // Cambiamos el estado
      digitalWrite(FLASH_LED_PIN, flashState); // Escribimos el nuevo estado en el pin
    }

    if (text == "/uxga" || text == "/photo") {

      fb = NULL; // Borramos fb
      sensor_t * s = esp_camera_sensor_get(); // Obtenemos sensor_t ejecutando la función
      s->set_framesize(s, FRAMESIZE_UXGA);    // uxga 1600x1200 100 kb, tamaño del fotograma

      fb = esp_camera_fb_get(); // Obtenemos fb
      esp_camera_fb_return(fb); // Devolvemos fb
      fb = esp_camera_fb_get(); // Obtenemos fb
      esp_camera_fb_return(fb); // Devolvemos fb
      
// ----------------------------------------
// Obtener una foto/fotograma con la cámara
// ----------------------------------------

      fb = esp_camera_fb_get();
      if (!fb) {
        Serial.println("Error de captura en la cámara");
        bot.sendMessage(chat_id, "Error de captura en la cámara", "");
        return;
      }

      currentByte = 0;
      fb_length = fb->len;
      fb_buffer = fb->buf;
    
    if (text == "/qvga") {

      fb = NULL;

      sensor_t * s = esp_camera_sensor_get();
      s->set_framesize(s, FRAMESIZE_QVGA);  // Formato  qvga 320x250   4 kb

      fb = esp_camera_fb_get();  
      esp_camera_fb_return(fb);
      fb = esp_camera_fb_get();
      esp_camera_fb_return(fb);

      Serial.println("\n\n\nEnviando formato QVGA");

      fb = esp_camera_fb_get(); // Obtiene una captura
      if (!fb) {
        Serial.println("Error de captura en la cámara");
        bot.sendMessage(chat_id, "Error de captura en la cámara", "");
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

    if (text == "/start") {
      String welcome = "Bienvenido a ESP32Cam Telegram bot.\n\n";
      welcome += "/photo : obtener una foto formato 1600x1200\n";    
      welcome += "/uxga : obtener una foto formato 1600x1200\n";
      welcome += "/flash : herramienta flash LED (¡Iluminación!)\n";
      welcome += "/qvga : obtener foto formato 320x250\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(FLASH_LED_PIN, OUTPUT);
  digitalWrite(FLASH_LED_PIN, flashState); // Apagado por defecto

  if (!setupCamera()) {
    Serial.println("¡Error en la configuración de la cámara!");
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
  Serial.println("Establecida la conexión WiFi");
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  bot.longPoll = 60; // Espera 60 segundos por un nuevo mensaje

  client.setInsecure();
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("Obteniendo respuesta");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}
