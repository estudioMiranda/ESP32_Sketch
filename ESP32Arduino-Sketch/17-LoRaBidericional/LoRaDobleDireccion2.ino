/*
  Doble cumunicación con LoRa
  Emite un mensaje cada medio segundo y está a la escucha de mensajes entrantes.
  Implementa un esquema de direccionamiento de un byte, con 0xFF como dirección de transmisión. 
  Utiliza readString () de la clase Stream para leer la carga útil. 
  El tiempo de espera de la clase Stream puede afectar otras funciones.
*/
// Bibliotecas LoRa
#include <SPI.h>              
#include <LoRa.h>    //https://github.com/sandeepmistry/arduino-LoRa
#include "SSD1306.h" //https://github.com/ThingPulse/esp8266-oled-ssd1306

//Pines para TTGO LoRa32 V1.0 OLED
#define SX1278_SCK  5    // GPIO5  -- SX1278's SCK
#define SX1278_MISO 19   // GPIO19 -- SX1278's MISO
#define SX1278_MOSI 27   // GPIO27 -- SX1278's MOSI
#define SX1278_CS   18   // GPIO18 -- SX1278's CS
#define SX1278_RST  14   // GPIO14 -- SX1278's RESET
#define SX1278_DI0  26   // GPIO26 -- SX1278's IRQ(Interrupt Request)

//Pines pantalla OLED
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // OLED ancho de pantalla, en pixels
#define SCREEN_HEIGHT 64 // OLED alto de pantalla, en pixels

#define LORA_BAND   868E6 // LoRa Band (Europe)
#define OLED_ADDR   0x3c  // OLED's ADDRESS


/*
//////////////////CONFIGURACIÓN 1//////////////////////
byte direccionLocal = 8;      // Dirección del dispositivo
byte destino = 18;            // Destino del envío
int intervalo = 3000;         // Intervalo de envíos
String mensaje = "Hola LoRa"; // Mensaje
///////////////////////////////////////////////////////
*/


///////////////////CONFIGURACIÓN 2/////////////////////
byte direccionLocal = 18;      // Dirección del dispositivo
byte destino = 8;              // Destino del envío
int intervalo = 2000;          // Intervalo entre mensajes
String mensaje = "LoRa Hola!"; // Mensaje
///////////////////////////////////////////////////////

String mensajeSaliente;       // Mensaje
byte contadorMsj = 0;         // Contador de mensajes
long lastSendTime = 0;        // Hora del último mensaje

// Instancia de la pantalla OLES
SSD1306 display(OLED_ADDR, OLED_SDA, OLED_SCL);

// Configuración pantalla OLED
void printScreen() {
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.setColor(BLACK);
  display.fillRect(0, 0, 127, 30);
  display.display();

  display.setColor(WHITE);
  display.drawString(0, 00, "Doble comunicación LoRa " + String(direccionLocal));
  display.drawString(0, 10, "De: " + String(direccionLocal)
                          + "  para: " + String(destino)
                          + " nº: " + String(contadorMsj));
  display.drawString(0, 20, "Tx PKT: " + mensaje);
  display.display();
}

// Función enviarMensaje
void enviarMensaje(String mensajeSaliente) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destino);              // add destination address
  LoRa.write(direccionLocal);             // add sender address
  LoRa.write(contadorMsj);                 // add message ID
  LoRa.write(mensajeSaliente.length());        // add payload length
  LoRa.print(mensajeSaliente);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  printScreen();
  
  Serial.println("Enviando mensaje " + String(contadorMsj) + " para la dirección: "+ String(destino)
  + " mensaje saliente: " + String(mensajeSaliente));
  Serial.println("Mensaje: " + mensaje); 
  Serial.println();
  delay(1000);
  contadorMsj++;                           // increment message ID
}

void onReceive(int tamanoPaquete) {
  
  if (tamanoPaquete == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int receptor = LoRa.read();          // recipient address
  byte emisor = LoRa.read();            // sender address
  byte IdMensajeEntrante = LoRa.read();     // incoming msg ID
  byte longitudMensajeEntrante = LoRa.read();    // incoming msg length

  String mensajeEntrante = "";                 // payload of packet

  while (LoRa.available()) {            // can't use readString() in callback, so
    mensajeEntrante += (char)LoRa.read();      // add bytes one by one
  }

  if (longitudMensajeEntrante != mensajeEntrante.length()) {   // check length for error
    Serial.println("Error: no coincide con su longitud");
    mensajeEntrante = "Error de longitud en el mensaje";
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (receptor != direccionLocal && receptor != 0xFF) {
    Serial.println("Este mensaje no es para mí.");
    mensajeEntrante = "El mensaje no es para mí";
    return;                             // skip rest of function
  }

  display.setColor(BLACK);
  display.fillRect(0, 32, 127, 61);
  display.display();

  display.setColor(WHITE);
  display.drawLine(0,31,127,31);
  display.drawString(0, 32, "Rx PKT: " + mensajeEntrante);

  display.drawString(0, 42, "RSSI: " + String(LoRa.packetRssi())
                          + " SNR: " + String(LoRa.packetSnr()));
  display.drawString(0, 52, "FR:"  + String(emisor)
                          + " TO:" + String(receptor)
                          + " LN:" + String(longitudMensajeEntrante)
                          + " ID:" + String(IdMensajeEntrante));
  display.display();

  // Impresión de los detalles de emisión/recepción de los envíos
  Serial.println("Recibido desde: 0x" + String(emisor, HEX));
  Serial.println("Enviado a: 0x" + String(receptor, HEX));
  Serial.println("ID mensaje: " + String(IdMensajeEntrante));
  Serial.println("Longitud del mensaje: " + String(longitudMensajeEntrante));
  Serial.println("Mensaje: " + mensajeEntrante);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();
  delay(1000);
}

void setup() {
  pinMode(OLED_RST,OUTPUT);
  digitalWrite(OLED_RST, LOW);  // Configura GPIO16 lOW para reiniciar la pantalla OLED
  delay(50);
  digitalWrite(OLED_RST, HIGH); // Cuando la pantalla OLED funciona el GPIO16 está HIGH
  delay(1000);

  display.init();
  display.flipScreenVertically();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.clear();

  Serial.begin(115200);                   // Baudios monitor serial
  while (!Serial);
  Serial.println("TTGO LoRa32 V1.0 P2P");
  display.drawString(0, 00, "TTGO LoRa32 V1.0 P2P");
  display.display();

  // SPI.begin(SX1278_SCK, SX1278_MISO, SX1278_MOSI, SX1278_CS);
  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(SX1278_CS, SX1278_RST, SX1278_DI0);  //  Configura CS, reset, IRQ pin

  if (!LoRa.begin(LORA_BAND))
  {             // initialize ratio at 868 MHz
    Serial.println("Fallo iniciando LoRa. Verifica las conexiones.");
    display.drawString(0, 10, "Fallo al iniciar LoRa");
    display.drawString(0, 20, "Verifica las conexiones");
    display.display();
    while (true);                       // Si falla, no hace nada
  }

  //LoRa.onReceive(onReceive);
  LoRa.receive();
  Serial.println("Comunicación LoRa inciada.");
  display.drawString(0, 10, "LoRa iniciado.");
  display.display();
  delay(1500);
  display.clear();
  display.display();
}

void loop() {
  if (millis() - lastSendTime > intervalo) {
    enviarMensaje(mensaje);
    lastSendTime = millis();            // timestamp the message
    intervalo = random(2000) + 1000;     // 2-3 seconds
    LoRa.receive();                     // go back into receive mode
  }
  int tamanoPaquete = LoRa.parsePacket();
  if (tamanoPaquete) { onReceive(tamanoPaquete);  }
}
