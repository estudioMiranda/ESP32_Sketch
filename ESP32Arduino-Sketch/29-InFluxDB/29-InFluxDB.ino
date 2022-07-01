
// Bibliotecas
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

// Tipo de tarjeta
#if defined(ESP32)
  #include <WiFiMulti.h>
  WiFiMulti wifiMulti;
  #define DEVICE "ESP32"
#elif defined(ESP8266)
  #include <ESP8266WiFiMulti.h>
  ESP8266WiFiMulti wifiMulti;
  #define DEVICE "ESP8266"
  #define WIFI_AUTH_OPEN ENC_TYPE_NONE
#endif

// Conexiópn WiFi router 
#define WIFI_SSID "studiomiranda"
// Contraseña WiFi
#define WIFI_PASSWORD "88888888"

// InfluxDB v2 server url, e.g. https://eu-central-1-1.aws.cloud2.influxdata.com (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_URL "https://europe-west1-1.gcp.cloud2.influxdata.com"
// InfluxDB v2 server or cloud API token (Use: InfluxDB UI -> Data -> API Tokens -> Generate API Token)
#define INFLUXDB_TOKEN "knUhcPOh0LvEsJqne_SsvRKqvyA0Dsrxq8F3qb6Qmfq7iRpZFtc-_PHG7ncFsImbvy0vnoe_RRWTtp0bnU81DQ=="
// InfluxDB v2 organization id (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_ORG "444cuatro@gmail.com"
// InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define INFLUXDB_BUCKET "ESP32"

// Configuración zona horaria https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
//  Madrid: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

// Instancia de la clase InfluxDB client
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Lectura del sensor
Point sensorReadings("medidas");

// Instancia de la clase BME280 I2C
Adafruit_BME280 bme;

// Variables
float temperatura;
float humedad;
float presion;

// Detectando sensor BME280
void initBME(){
  if (!bme.begin(0x76)) {
    Serial.println("¡Sensor BME280 no dectado, compruebe los conectores!");
    while (1);
  }
}

void setup() {
  
  Serial.begin(115200);

  // WiFi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Iniciando conexión WiFi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  
  //Iniciado el sensor BME280 
  initBME();
  
  // Añadiendo etiquetas
  sensorReadings.addTag("dispositivo", DEVICE);
  sensorReadings.addTag("localización", "A Coruña");
  sensorReadings.addTag("sensor", "bme280");

  // Se necesita validación horaria para la certificación
  // Servidores NTP cercanos: https://www.pool.ntp.org/zone/
  // El progreso se imprime en el monitor serie.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Validando servidores
  if (client.validateConnection()) {
    Serial.print("Conexión establecida a InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("No se ha podido realizar la conexión a InfluxDB: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void loop() {
  // Lectura de sensores
  temperatura = bme.readTemperature();
  humedad = bme.readHumidity();
  presion = bme.readPressure()/100.0F;

  // Añadiendo campos al punto mensaje
  sensorReadings.addField("temperatura", temperatura);
  sensorReadings.addField("humedad", humedad);
  sensorReadings.addField("presion", presion);

  // Imprime los datos que envía
  Serial.print("Escribiendo: ");
  Serial.println(client.pointToLineProtocol(sensorReadings));
  
  // Escribe dentro del buffer
  client.writePoint(sensorReadings);

  // Limpia los campos para próximas lecturas.
  sensorReadings.clearFields();

  // Reconecta si la señal se pierde
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("Conexión WiFi perdida");
  }

  // Espera de 10 segundos
  Serial.println("Espera de 10 segundos");
  delay(10000);
}
