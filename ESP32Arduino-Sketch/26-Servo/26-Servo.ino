// Bibliotecas
#include <Servo.h>

// Instancia de la clase Servo (max 12 instancias)
Servo myservo;  

// Variable posición inicial
int pos = 0;

void setup() {
  // Pin señal del servo
  myservo.attach(13);
}

void loop() {
  for (pos = 0; pos <= 180; pos += 1) { // Desde Oº a 180º incrementando 1º
    myservo.write(pos);              // incrementa 1º la posición
    delay(4.5);                       // espera 15 milisegundos 
  }
  for (pos = 180; pos >= 0; pos -= 1) { // De 180º a 0º decrementando 1º
    myservo.write(pos);              
    delay(4.5);                       
  }
}
                   
