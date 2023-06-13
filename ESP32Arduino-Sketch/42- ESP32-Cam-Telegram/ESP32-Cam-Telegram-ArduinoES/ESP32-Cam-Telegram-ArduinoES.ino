/*
  ArduinoES
  Adaptación del skecht de: 
  https://github.com/jameszah/ESP32-CAM-Telegram    
*/

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

char ssid[] = "tuRed";            
char password[] = "tuContraseña";

// -----------------------------------
// Configura tu cuenta de Telegram BOT
// -----------------------------------

#define BOTtoken "6025685312:AAHJhZwGU82yaI_xB4b1pMcyQ7I66hJjC8o"
//#define chat_id "495285388"

// -------------------------------------
// Incluye los archivos de configuración
// -------------------------------------

#include "camera_pins.h"
#include "camera_code.h"

// -----------------------
// Pin del flash y botones
// -----------------------

#define FLASH_LED_PIN 4
#define BUTTON 15

// ---------------------
// Instancias de cliente
// ---------------------

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// ---------
// Variables 
// ---------

int Bot_mtbs = 1000; // Tiempo de espera entre escaneos
long Bot_lasttime;   // Último escaneo guardado

bool flashState = LOW;

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

// -------------------------
// Manejando nuevos mensajes
// -------------------------

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    Serial.printf("\nObteniendo un mensaje %s\n", text);

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Invitado";

    String hi = "Hola ... \nGot: ";
    hi += text;
    bot.sendMessage(chat_id, hi, "Markdown");

    if (text == "/flash") {
      flashState = !flashState;
      digitalWrite(FLASH_LED_PIN, flashState);
    }
    
// ------------------------------------------------------
// Formato OV5640 => QSXGA => 2560×2048 px => 5 megapixel
// Formato OV3660 =>  QXGA => 2048x1536 px => 3 megapixel
// Formato OV2640 =>  UXGA => 1600×1200 px => 2 megapixel
// ------------------------------------------------------

/////////////////////////////////////////////////////////

    if (text == "/qsxga;") {

      fb = NULL;

      sensor_t * s = esp_camera_sensor_get();
      s->set_framesize(s, FRAMESIZE_QSXGA);

      fb = esp_camera_fb_get();   // Quita dos fotos en negro
      esp_camera_fb_return(fb);
      fb = esp_camera_fb_get();
      esp_camera_fb_return(fb);

     Serial.println("\n\n\nEnviando QSXGA");
      fb = esp_camera_fb_get();
      // Obteniendo una captura
      
      if (!fb) {
        Serial.println("Error de captura");
        bot.sendMessage(chat_id, "Error de captura", "");
        return;
      }

      currentByte = 0;
      fb_length = fb->len;
      fb_buffer = fb->buf;

/////////////////////////////////////////////////////////

      if (text == "/caption") {
        Serial.println("\n\n\nSending UXGA with caption");
        Serial.println("\n>>>>> Sending as 512 byte blocks, with a caption, and with jzdelay of 0, bytes=  " + String(fb_length));

        String sent = bot.sendMultipartFormDataToTelegramWithCaption("sendPhoto", "photo", "img.jpg",
                      "image/jpeg", "I am the caption!", chat_id, fb_length,
                      isMoreDataAvailable, getNextByte, nullptr, nullptr);

        Serial.println("done!");
      } else {

        Serial.println("\n\n\nSending UXGA");
        Serial.println("\n>>>>> Sending as 512 byte blocks, with jzdelay of 0, bytes=  " + String(fb_length));

        bot.sendPhotoByBinary(chat_id, "image/jpeg", fb_length,
                              isMoreDataAvailable, getNextByte,
                              nullptr, nullptr);

        dataAvailable = true;
   
        Serial.println("done!");
      }

      esp_camera_fb_return(fb);
    }

/////////////////////////////////////////////////////////

 if (text == "/qxga" ) {

      fb = NULL;

      sensor_t * s = esp_camera_sensor_get();
      s->set_framesize(s, FRAMESIZE_QXGA);

      fb = esp_camera_fb_get();
      esp_camera_fb_return(fb);
      fb = esp_camera_fb_get();
      esp_camera_fb_return(fb);

      Serial.println("\n\n\nSending QXGA");

      fb = esp_camera_fb_get();
      if (!fb) {
        Serial.println("Camera capture failed");
        bot.sendMessage(chat_id, "Camera capture failed", "");
        return;
      }

      currentByte = 0;
      fb_length = fb->len;
      fb_buffer = fb->buf;

      Serial.println("\n>>>>> Sending as 512 byte blocks, with jzdelay of 0, bytes=  " + String(fb_length));

      bot.sendPhotoByBinary(chat_id, "image/jpeg", fb_length,
                            isMoreDataAvailable, getNextByte,
                            nullptr, nullptr);
      
      esp_camera_fb_return(fb);
    }

/////////////////////////////////////////////////////////

    if (text == "/uxga" ) {

      fb = NULL;

      sensor_t * s = esp_camera_sensor_get();
      s->set_framesize(s, FRAMESIZE_UXGA);

      fb = esp_camera_fb_get();
      esp_camera_fb_return(fb);
      fb = esp_camera_fb_get();
      esp_camera_fb_return(fb);

      Serial.println("\n\n\nSending UXGA");

      fb = esp_camera_fb_get();
      if (!fb) {
        Serial.println("Camera capture failed");
        bot.sendMessage(chat_id, "Camera capture failed", "");
        return;
      }

      currentByte = 0;
      fb_length = fb->len;
      fb_buffer = fb->buf;

      Serial.println("\n>>>>> Sending as 512 byte blocks, with jzdelay of 0, bytes=  " + String(fb_length));

      bot.sendPhotoByBinary(chat_id, "image/jpeg", fb_length,
                            isMoreDataAvailable, getNextByte,
                            nullptr, nullptr);
      
      esp_camera_fb_return(fb);
    }

    if (text == "/start") {
      String welcome = "Bienvenido a ESP32Cam Telegram bot.\n\n";
      welcome += "/qsxga : OV5640 => QSXGA => 2560×2048 px => 5 megapixel\n";
      welcome += "/sxga : OV3660 =>  QXGA => 2048x1536 px => 3 megapixel\n";
      welcome += "/uxga : OV2640 =>  UXGA => 1600×1200 px => 2 megapixel\n";
      welcome += "/flash : Activa el flash LED\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

 

void setup() {
  Serial.begin(115200);

  pinMode(FLASH_LED_PIN, OUTPUT);
  digitalWrite(FLASH_LED_PIN, flashState); //defaults to low

  if (!setupCamera()) {
    Serial.println("Camera Setup Failed!");
    while (true) {
      delay(100);
    }
  }

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Make the bot wait for a new message for up to 60seconds
  bot.longPoll = 60;

  client.setInsecure();
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}
