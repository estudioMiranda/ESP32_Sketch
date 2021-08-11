#include <Motor.h>
Motor motor = Motor(27,26,25,33);

int ledPin = 2;

int freq = 5000;
int ledChannel = 0;
int resolution = 8;

int dutyCycle = 255;

void setup() {

  pinMode(ledPin, OUTPUT);
  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(ledPin, ledChannel);
  }

void loop()
{
    ledcWrite(ledChannel, dutyCycle);
    motor.Avanzar();
    delay(1000);
    motor.GirarDerecha();
    delay(1000);
    motor.GirarIzquierda();
    delay(1000);
    motor.Retroceder();
    delay(1000);
    motor.Parar();
    delay(4000);
}
