// Bibliotecas
#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Tipo de tarjeta
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif


// Proporciona el proceso de generación del TOKEN
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// 1. Credenciales conexión WiFi
#define WIFI_SSID "studio"
#define WIFI_PASSWORD "88888888"

// 2. Define clave de API web (configuración del proyecto)
#define API_KEY "AIzaSyB7tafxgr8RYdLszVs_L7K4Q2fs39E-jXg"

// 3. Define RTDB (real time data base) URL
#define DATABASE_URL "https://esp32-1021b-default-rtdb.europe-west1.firebasedatabase.app/" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

// 4. Define el correo y contraseña del usuario registrado (Autentificación por e-mail)
#define USER_EMAIL "maria@correo.es"
#define USER_PASSWORD "12345678"

// Instancias de la clases de firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variable USER UID
String uid;

// Variables para la base de datos
String databasePath;
String tempPath;
String humPath;
String presPath;

// Variables BME280 sensor
Adafruit_BME280 bme; // I2C
float temperature;
float humidity;
float pressure;

// Variables de tiempo (envia nuevas lecturas cada  5 segundos)
unsigned long sendDataPrevMillis = 0;
unsigned long timerDelay = 5000;

// Inicia el sensore BME280
void initBME(){
  if (!bme.begin(0x76)) {
    Serial.println("¡No se encuentra el sensor BME280, revisa las conexiones!");
    while (1);
  }
}

// Inicia la conexión WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Escribe los valores para enviar a la base de datos
void sendFloat(String path, float value){
  if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value)){
    Serial.print("Escribiendo valor: ");
    Serial.print (value);
    Serial.print(" en el siguiente dirección: ");
    Serial.println(path);
    Serial.println("TRANSMITIDO");
    Serial.println("DIRECCIÓN: " + fbdo.dataPath());
    Serial.println("TIPO: " + fbdo.dataType());
  }
  else {
    Serial.println("FALLO DE TRANSMISIÓN");
    Serial.println("TIPO DE ERROR: " + fbdo.errorReason());
  }
}

void setup(){
  Serial.begin(115200);

  // Inicia el sensor BME280
  initBME();
  initWiFi();

  // Asigna la api key (requerida)
  config.api_key = API_KEY;

  // Asigna las credenciales del usuario
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Asigna la base de datos RTDB URL (required)
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);
  fbdo.setResponseSize(4096);

  // Asigna la respuesta de la función 
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Asigna el reintento maximo de la generación de token
  config.max_token_generation_retry = 5;

  // Inicia la biblioteca de firebase configuración y autentificación
  Firebase.begin(&config, &auth);

  // Obteniendo el UID del usuario puede tardar unos segundos
  Serial.println("Obteniendo el UID del usuario");
  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
  }
  // Imprime el UID del usuario
  uid = auth.token.uid.c_str();
  Serial.print("UID de usuario: ");
  Serial.println(uid);

  // Actualiza la dirección de la base de datos
  databasePath = "/UsersData/" + uid;

  // Actualiza las direcciones con los datos de los sensores
  tempPath = databasePath + "/temperature"; // --> UsersData/<user_uid>/temperature
  humPath = databasePath + "/humidity"; // --> UsersData/<user_uid>/humidity
  presPath = databasePath + "/pressure"; // --> UsersData/<user_uid>/pressure
}

void loop(){
  // Envía nuevas lecturas a la base de datos
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();

    // Obtiene la nuevas lecturas
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    pressure = bme.readPressure()/100.0F;

    // Envía las nuevas lecturas a la base de datos
    sendFloat(tempPath, temperature);
    sendFloat(humPath, humidity);
    sendFloat(presPath, pressure);
  }
}
