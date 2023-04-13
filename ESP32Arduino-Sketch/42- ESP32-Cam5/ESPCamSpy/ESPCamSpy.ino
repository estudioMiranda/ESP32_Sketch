/*******************************************************************

   A Telegram bot for taking a photo with an ESP32Cam
    See line 250 of UniversalTelegramBot.cpp in the current github software
    https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/issues/235#issue-842397567
    See https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/65e6f826cbab242366d69f00cebc25cdd1e81305/src/UniversalTelegramBot.cpp#L250

Using library WiFi 
Using library WiFiClientSecure at version 1.0 
Using library ArduinoJson at version 6.16.1 
    
*******************************************************************/

// ----------------------------
// Standard Libraries - Already Installed if you have ESP32 set up
// ----------------------------

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "esp_camera.h"

//#include <UniversalTelegramBot.h>
#include "UniversalTelegramBot.h"
// https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot

#include <ArduinoJson.h>
// https://github.com/bblanchon/ArduinoJson

//------- Replace the following! ------

//#define CAMERA_MODEL_WROVER_KIT
#define CAMERA_MODEL_ESP_EYE
//#define CAMERA_MODEL_M5STACK_PSRAM
//#define CAMERA_MODEL_M5STACK_WIDE
//#define CAMERA_MODEL_AI_THINKER

// Initialize Wifi connection to the router
char ssid[] = "esp32";
char password[] = "88888888";



// Initialize Telegram BOT
#define BOTtoken "6025685312:AAHJhZwGU82yaI_xB4b1pMcyQ7I66hJjC8o" 

#include "camera_pins.h"
#include "camera_code.h"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; // tiempo entre mensajes escaneado
long Bot_lasttime;   // tiempo ultimo mensaje escaneado

camera_fb_t * fb = NULL;

bool isMoreDataAvailable();

///////////////// envía fotos en bloques de 512 byte
int currentByte;
uint8_t* fb_buffer;
size_t fb_length;

bool isMoreDataAvailable() {
  return (fb_length - currentByte);
}

uint8_t getNextByte() {
  currentByte++;
  return (fb_buffer[currentByte - 1]);
}

bool dataAvailable = false;

//////////////////// envia fotos en 1 solo bloque

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

///////////////////////////////


void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    Serial.printf("\nObteniendo un mensaje %s\n", text);

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Invitado";

    String hi = "Hola ... \nObteniendo: ";
    hi += text;
    bot.sendMessage(chat_id, hi, "Markdown");

    if (text == "/uxga" || text == "/photo" || text == "/caption" ) {

      fb = NULL;

      sensor_t * s = esp_camera_sensor_get();
      s->set_framesize(s, FRAMESIZE_UXGA);  // jz  uxga 1600x1200 100 kb

      fb = esp_camera_fb_get();
      esp_camera_fb_return(fb);
      fb = esp_camera_fb_get();
      esp_camera_fb_return(fb);

      // Take Picture with Camera
      fb = esp_camera_fb_get();
      if (!fb) {
        Serial.println("Ha ocurrido un error en la captura");
        bot.sendMessage(chat_id, "Ha ocurrido un error en la captura", "");
        return;
      }

      currentByte = 0;
      fb_length = fb->len;
      fb_buffer = fb->buf;

      if (text == "/caption") {
        Serial.println("\n\n\nEnviando captura de imagen UXGA");
        Serial.println("\n>>>>> Enviando bloques de 512 byte, de una captura de imagen, y con un jzdelay de 0, bytes=  " + String(fb_length));

        String sent = bot.sendMultipartFormDataToTelegramWithCaption("sendPhoto", "photo", "img.jpg",
                      "image/jpeg", "¡Soy una captura de imagen!", chat_id, fb_length,
                      isMoreDataAvailable, getNextByte, nullptr, nullptr);

        Serial.println("¡finalizada!");
      } else {

        Serial.println("\n\n\nEnviando UXGA");
        Serial.println("\n>>>>> Enviando bloques de 512 byte, con jzdelay de 0, bytes=  " + String(fb_length));

        bot.sendPhotoByBinary(chat_id, "image/jpeg", fb_length,
                              isMoreDataAvailable, getNextByte,
                              nullptr, nullptr);

        dataAvailable = true;
        Serial.println("¡finalizada!");
      }

      esp_camera_fb_return(fb);
    }



    if (text == "/qvga" ) {

      fb = NULL;

      sensor_t * s = esp_camera_sensor_get();
      s->set_framesize(s, FRAMESIZE_QVGA);  // jz  qvga 320x250   4 kb

      fb = esp_camera_fb_get();   // takes a pic or two for camera to adjust
      esp_camera_fb_return(fb);
      fb = esp_camera_fb_get();
      esp_camera_fb_return(fb);

      Serial.println("\n\n\nEnviando QVGA");

      // Take Picture with Camera
      fb = esp_camera_fb_get();
      if (!fb) {
        Serial.println("Ha ocurrido un error en la captura");
        bot.sendMessage(chat_id, "Ha ocurrido un error en la captura", "");
        return;
      }

      currentByte = 0;
      fb_length = fb->len;
      fb_buffer = fb->buf;

      Serial.println("\n>>>>> Enviando bloques de 512 byte, con jzdelay de 0, bytes=  " + String(fb_length));

      bot.sendPhotoByBinary(chat_id, "image/jpeg", fb_length,
                            isMoreDataAvailable, getNextByte,
                            nullptr, nullptr);
      
      esp_camera_fb_return(fb);
    }

    if (text == "/start") {
      String welcome = "Bienvenido a ESP32Cam Telegram bot.\n\n";
      welcome += "/photo : Obtendrá una foto\n";
      welcome += "/qvga : Obtendraá una foto en baja resolución \n";
      welcome += "/uxga : Obtendrá una foto en media resolución\n";
      welcome += "/caption : Obtendra una foto en alta resolución\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

 

void setup() {
  Serial.begin(115200);

  if (!setupCamera()) {
    Serial.println("¡Ha ocurrido un error en la configuración de la cámara!");
    while (true) {
      delay(100);
    }
  }

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("Conexión WiFi establecida");
  Serial.print("Dirección IP ESP32Cam: ");
  Serial.println(WiFi.localIP());

  // Make the bot wait for a new message for up to 60seconds
  bot.longPoll = 60;

  client.setInsecure();
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("obteniendo respuesta");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}
