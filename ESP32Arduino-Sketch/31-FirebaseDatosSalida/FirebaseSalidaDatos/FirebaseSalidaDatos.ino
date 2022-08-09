// Bibliotecas
#include <Arduino.h>
#include <Firebase_ESP_Client.h>

// Tipo tarjeta
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif


// Realiza el proceso de generación de la información para el token
#include "addons/TokenHelper.h"
// Imprime los datos de la RTDB y otras funciones auxiliares
#include "addons/RTDBHelper.h"

// Introduce tus credenciales de conexión WiFi
#define WIFI_SSID "REPLACE_WITH_YOUR_SSID"
#define WIFI_PASSWORD "REPLACE_WITH_YOUR_PASSWORD"

// Introduce tu "Clave de API web"
#define API_KEY "REPLACE_WITH_YOUR_PROJECT_API_KEY"

// Introduce tu Autentificación correo electrónico y contraseña
#define USER_EMAIL "REPLACE_WITH_THE_USER_EMAIL"
#define USER_PASSWORD "REPLACE_WITH_THE_USER_PASSWORD"

// Introduce tu URL de la RealTimeDataBase
#define DATABASE_URL "REPLACE_WITH_YOUR_DATABASE_URL"

// Intancias de la clase FirebaseData, FirebaseAuth y FirebaseConfig
FirebaseData stream;
FirebaseAuth auth;
FirebaseConfig config;

// Variables que guardan las rutas de la base de datos
String listenerPath = "tarjeta/led/digital/";

// Asigna el pin
const int led = 2;


// Función conexión WiFi
void initWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  Serial.println();
}

// Función que recibe los cambios en la base de datos
void streamCallback(FirebaseStream data){
  Serial.printf("stream path, %s\nevent path, %s\ndata type, %s\nevent type, %s\n\n",
                data.streamPath().c_str(),
                data.dataPath().c_str(),
                data.dataType().c_str(),
                data.eventType().c_str());
  printResult(data); //see addons/RTDBHelper.h
  Serial.println();

  // Obtiene las direcciones y dispara la función
  String streamPath = String(data.dataPath());

  // Si los datos devuelven un entero, hay un cambio en el estado la dirección del GPIO 
  if (data.dataTypeEnum() == fb_esp_rtdb_data_type_integer){
    String gpio = streamPath.substring(1);
    int state = data.intData();
    Serial.print("GPIO: ");
    Serial.println(gpio);
    Serial.print("STATE: ");
    Serial.println(state);
    digitalWrite(gpio.toInt(), state);
  }

  /* Cuando se ejecuta por primera vez, se activa en la ruta raíz (/) y devuelve un JSON con todas las claves y valores de esa ruta. 
  Entonces, podemos obtener todos los valores de la base de datos y actualizar los estados de GPIO*/
  if (data.dataTypeEnum() == fb_esp_rtdb_data_type_json){
    FirebaseJson json = data.to<FirebaseJson>();

    // Para iterar todos los valores en el objeto Json
    size_t count = json.iteratorBegin();
    Serial.println("\n---------");
    for (size_t i = 0; i < count; i++){
        FirebaseJson::IteratorValue value = json.valueAt(i);
        int gpio = value.key.toInt();
        int state = value.value.toInt();
        Serial.print("STATE: ");
        Serial.println(state);
        Serial.print("GPIO:");
        Serial.println(gpio);
        digitalWrite(gpio, state);
        Serial.printf("Name: %s, Value: %s, Type: %s\n", value.key.c_str(), value.value.c_str(), value.type == FirebaseJson::JSON_OBJECT ? "object" : "array");
    }
    Serial.println();
    json.iteratorEnd(); // Se necesita libre la memoria utilizada en la iteración (colección de datos del nodo)
  }
  // Este es el tamaño de los datos de la transmisión recibida (valor actual y máximo) 
  // El tamaño máximo de los datos es el tamaño de los datos en la ruta de la transmisión desde que la transmisión se conectó 
  // y se leyó una vez y no se actualizará hasta que se produzca la reconexión de la transmisión. 
  // Este valor máximo será cero ya que no se recibió carga útil en el caso de ESP8266 que 
  // BearSSL reservó el tamaño del búfer Rx es menor que la carga útil de flujo real.
  
  Serial.printf("Received stream payload size: %d (Max. %d)\n\n", data.payloadLength(), data.maxPayloadLength());
}

void streamTimeoutCallback(bool timeout){
  if (timeout)
    Serial.println("fin de la transmisión, resumen...\n");
  if (!stream.httpConnected())
    Serial.printf("error de código: %d, motivo: %s\n\n", stream.httpCode(), stream.errorReason().c_str());
}

void setup(){
  Serial.begin(115200);
  initWiFi();

  // Pin configurado como salida
  pinMode(led, OUTPUT);
  

  
  // Asigna la Clave de API web
  config.api_key = API_KEY;

  // Asigna el usuario su correo y contraseña
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Asigna la URL de la RealTimeBasaData
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);

  // Asigna la función de respuesta para la tarea de generación de tokens de ejecución prolongada
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  // Asigna el reintento máximo de la generación de tokens
  config.max_token_generation_retry = 5;

  // Inicialice la biblioteca con la autenticación y configuración de Firebase
  Firebase.begin(&config, &auth);

  // Transmisión (siempre que los datos cambien en una ruta) 
  // Comenzar transmisión en una ruta de base de datos --> tarjeta/led/digital
  if (!Firebase.RTDB.beginStream(&stream, listenerPath.c_str()))
    Serial.printf("Error al iniciar la transmisión, %s\n\n", stream.errorReason().c_str());

  // Asigne una función de respuesta para que se ejecute cuando detecte cambios en la base de datos
  Firebase.RTDB.setStreamCallback(&stream, streamCallback, streamTimeoutCallback);

  delay(2000);
}

void loop(){

}
