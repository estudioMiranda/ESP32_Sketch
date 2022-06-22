// Incluir Biblioteca
#include <Stepper.h>

const int stepsPerRevolution = 2048;  // Nº de pasos por revolución/vuelta

// ULN2003 Pines del Driver del Motor (puente H)
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17

// Instancia de la clase Stepper
Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

void setup() {
  Serial.begin(115200);
  
  // Configuramos la velocidad a 5 rpm (revoluciones/vueltas por minuto)
  myStepper.setSpeed(5);  
}

void loop() {
  // step one revolution in one direction:
  Serial.println("Sentido horario");
  myStepper.step(stepsPerRevolution);
  delay(1000);

  // step one revolution in the other direction:
  Serial.println("Sentido antihorario");
  myStepper.step(-stepsPerRevolution);
  delay(1000);
}
