#include "Motor.h"

Motor::Motor( uint8_t _motor1Pin1,
              uint8_t _motor1Pin2,
              uint8_t _motor2Pin1,
              uint8_t _motor2Pin2
            )

{
  motor1Pin1=_motor1Pin1;
  motor1Pin2=_motor1Pin2;
  motor2Pin1=_motor2Pin1;
  motor2Pin2=_motor2Pin2;
  
  pinMode(motor1Pin1,OUTPUT);
  pinMode(motor1Pin2,OUTPUT);
  pinMode(motor2Pin1,OUTPUT);
  pinMode(motor2Pin2,OUTPUT);

}

void Motor::Avanzar()
{
  digitalWrite(motor1Pin1,HIGH);
  digitalWrite(motor1Pin2,LOW);
  digitalWrite(motor2Pin1,HIGH);
  digitalWrite(motor2Pin2,LOW);
}
void Motor::Retroceder()
{
  digitalWrite(motor1Pin1,LOW);
  digitalWrite(motor1Pin2,HIGH);
  digitalWrite(motor2Pin1,LOW);
  digitalWrite(motor2Pin2,HIGH);
}
void Motor::GirarIzquierda()
{
  digitalWrite(motor1Pin1,HIGH);
  digitalWrite(motor1Pin2,LOW);
  digitalWrite(motor2Pin1,LOW);
  digitalWrite(motor2Pin2,HIGH);
}
void Motor::GirarDerecha()
{
  digitalWrite(motor1Pin1,LOW);
  digitalWrite(motor1Pin2,HIGH);
  digitalWrite(motor2Pin1,HIGH);
  digitalWrite(motor2Pin2,LOW);
}

void Motor::Parar()
{
  digitalWrite(motor1Pin1,LOW);
  digitalWrite(motor1Pin2,LOW);
  digitalWrite(motor2Pin1,LOW);
  digitalWrite(motor2Pin2,LOW);
}

