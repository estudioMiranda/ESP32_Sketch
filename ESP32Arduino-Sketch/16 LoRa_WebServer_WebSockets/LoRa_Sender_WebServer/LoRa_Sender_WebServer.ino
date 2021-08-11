/*********
  StudioMiranda Arduinoes
  WebSocket ServidorWeb
*********/

//  Bibliotaca comunicación LoRa
#include <SPI.h>
#include <LoRa.h>

//  Biblietecas pantalla OLED "SSD1306"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//  Biblieteca sensor BME280, temperatura, humedad y presión
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Pines transmisión/recepción TTGO LoRa v.1.0
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//  433E6 Banda Asia
//  866E6 Banda Europa
//  915E6 Banda Norteamérica
#define BAND 866E6

// Pines pantalla OLED 
#define OLED_SDA 4
#define OLED_SCL 15 
#define OLED_RST 16
#define SCREEN_WIDTH 128 // Pantalla OLED ancho, en pixeles
#define SCREEN_HEIGHT 64 // Pantalla OLED alto, en pixeles

// Pines BME280
#define SDA 16
#define SCL 17

// Conexión I2C 
TwoWire I2Cone = TwoWire(1);

// Instancia sensor
Adafruit_BME280 bme;

//  Contador de paquetes (ID)
int readingID = 0;

int counter = 0;

String LoRaMessage = "";

// Variables datos sensor bme
float temperature = 0;
float humidity = 0;
float pressure = 0;

// Instancia de pantalla OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

// Inicia la pantalla OLED
void startOLED(){
  // Reinicio pantalla
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  // Inicio pantalla
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C, false, false)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Emisor LoRa");
}

// Inicio del módulo emisor/receptor LoRa
void startLoRa(){
  // Pines SPI LoRa
  SPI.begin(SCK, MISO, MOSI, SS);
  
  // Configuración módulo
  LoRa.setPins(SS, RST, DIO0);

  // Espera 5 segundos
  while (!LoRa.begin(BAND) && counter < 10) {
    Serial.print(".");
    counter++;
    delay(500);
  }
  if (counter == 10) {
    // Incrementa el readingID
    readingID++;
    Serial.println("¡Fallo de inicio LoRa!"); 
  }
  Serial.println("¡LoRa iniciado, OK!");
  display.setCursor(0,10);
  display.clearDisplay();
  display.print("¡LoRa iniciado, OK!");
  display.display();
  delay(2000);
}

//  Concexión I2C (Inter-Integrated Circuit)
//  I2C requiere únicamente dos cables para su funcionamiento
//  Uno para la señal de reloj CLK y otro para el envío de datos SDA
void startBME(){
  I2Cone.begin(SDA, SCL, 100000); 
  bool status1 = bme.begin(0x76, &I2Cone);  
  if (!status1) {
    Serial.println("No se reciben valores, revisa los cables de conexión");
    while (1);
  }
}

// Obtiene las lecturas del sensor
void getReadings(){
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = bme.readPressure() / 100.0F;
}

// Envia los datos en paquetes/mensajes
void sendReadings() {
  
// Mensaje
  LoRaMessage = String(readingID) + "/" + String(temperature) + "&" + String(humidity) + "#" + String(pressure);
  
 // Envío del mensaje
  LoRa.beginPacket();
  LoRa.print(LoRaMessage);
  LoRa.endPacket();

  // Muestra en pantalla OLED los datos
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.print("¡Paquete LoRa enviado!");
  display.setCursor(0,20);
  display.print("Temperatura:");
  display.setCursor(72,20);
  display.print(temperature);
  display.setCursor(0,30);
  display.print("Humedad:");
  display.setCursor(54,30);
  display.print(humidity);
  display.setCursor(0,40);
  display.print("Presión:");
  display.setCursor(54,40);
  display.print(pressure);
  display.setCursor(0,50);
  display.print("Reading ID:");
  display.setCursor(66,50);
  display.print(readingID);
  display.display();
  readingID++;
}

void setup() {
  // Baudios monitor serie
  Serial.begin(115200);
  // LLamada funciones pantalla, sensor y módulo LoRa
  startOLED();
  startBME();
  startLoRa();
}
void loop() {
  // Llamada funciones obtener lecturas y enviar lecturas
  getReadings();
  sendReadings();
  delay(10000);
}
