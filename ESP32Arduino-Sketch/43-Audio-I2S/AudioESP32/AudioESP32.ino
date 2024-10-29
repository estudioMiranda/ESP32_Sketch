/*
  ESP32 I2S Ejemplo de micrófono
  
  Ejemplo de sonido desde micrófono I2S.
  Visualización en el Serial Plotter
  Requiere de un micrófono I2S INMP441
  Adaptado para mayor claridad y mejoras en el manejo de errores.
*/

// Incluir el I2S driver
#include <driver/i2s.h>

// Conexiones del micrófono INMP441 I2S
#define I2S_WS 25   // Canal
#define I2S_SD 33   // Datos
#define I2S_SCK 32  // Relog

// Puerto I"S
#define I2S_PORT I2S_NUM_0

// Variables
const int sampleRate = 44100;        // Tasa de muestreo para audio
const int bufferLen = 64;            // Longitud del búfer de entrada
const int dmaBufCount = 8;           // Número de búferes DMA
int16_t sBuffer[bufferLen];
int rangelimit = 3000;               // Sensibilidad para el Serial Plotter

void i2s_install() {
  // Configurar ajustes de I2S
  const i2s_config_t i2s_config = {
    .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = sampleRate,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = dmaBufCount,
    .dma_buf_len = bufferLen,
    .use_apll = false
  };

  // Instalar y comprobar si hay errores
  if (i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL) != ESP_OK) {
    Serial.println("Fallo de instalación del driver I2S");
  }
}

void i2s_setpin() {
  // Configurar asignación de pines I2S
  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,   // Not used
    .data_in_num = I2S_SD
  };

  // Establecer pines y verificar si hay errores
  if (i2s_set_pin(I2S_PORT, &pin_config) != ESP_OK) {
    Serial.println("Error al configurar la asignación de pines I2S");
  }
}

void setup() {
  // Iniciarlizar el monitor serial
  Serial.begin(115200);
  delay(1000);
  
  // Inicializar la comunicación I2S
  i2s_install();
  i2s_setpin();
  i2s_start(I2S_PORT);
  delay(500);

  Serial.println("Inicialización del micrófono I2S completada");
}

void loop() {
  // Rango de sensibilidad del Serial Plotter
  Serial.printf("%d %d ", -rangelimit, rangelimit);

  // Leer datos del I2S
  size_t bytesRead = 0;
  if (i2s_read(I2S_PORT, &sBuffer, bufferLen * sizeof(int16_t), &bytesRead, portMAX_DELAY) == ESP_OK) {
    int16_t samplesRead = bytesRead / sizeof(int16_t);

    // Calcular el valor medio de las muestras
    float mean = 0.0;
    for (int i = 0; i < samplesRead; ++i) {
      mean += sBuffer[i];
    }
    mean /= samplesRead;

    // Imprimir valor medio para el Serial Plotter
    Serial.println(mean);
  } else {
    Serial.println("Error al leer datos del micrófono I2S");
  }
}
