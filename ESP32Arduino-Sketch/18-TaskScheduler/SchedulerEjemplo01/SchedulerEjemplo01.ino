// Prueba
// Inica dos tareas, tarea1 y tarea2, que estan activadas
// La tarea1 se ejecuta cada 2 segundos durante 10 iteraciones y se para
// La tarea2 se ejetuca cada 3 segundos indefinidamente
// La tarea1 activa la tarea2 en la primera iteración
// La tarea3 se ejecuta cada 5 segundos
// loo() ejecuta por 1 segundo por defecto
// La tarea1 desactiva la tarea3 en la última iteración y modifica la tarea2 que se ejecuta ahora cada 1/2 segundos
// Como la tarea2 es menor que el intervalo por defecto el loop() cambia a 1/2 segundo
// Al final la tarea2 se ejecuta cada 1/2 segundo
 
#include <TaskScheduler.h>

// Funciones de las tareas
void funcionTarea1();
void funcionTarea2();
void funcionTarea3(); 

// Tarea    ( tiempo, veces, función ) 
Task tarea1 (2000, 10, &funcionTarea1);  // Añade una tarea
Task tarea2 (3000, TASK_FOREVER, &funcionTarea2);
Task tarea3 (5000, TASK_FOREVER, &funcionTarea3);

Scheduler organizador;

void funcionTarea1() {
    Serial.print("tarea1: ");
    Serial.println(millis());
    
    // Ejecuta la tarea3 en la primera iteración
    if (tarea1.isFirstIteration()) { 
      organizador.addTask(tarea3);  // Añade la tarea3
      tarea3.enable();              // Activa
      Serial.println("tarea1: activada la tarea3 en la 1º iteración, añadida a la cadena");
    }
    // Desactiva la tarea3 en la última iteración
    if (tarea1.isLastIteration()) {
      tarea3.disable();             // Desactiva la tarea3
      organizador.deleteTask(tarea3); // Elinina la tarea3
      tarea2.setInterval(500);
      Serial.println("Desactivada tarea3 en la última iteración de tarea1 y borrada de la cadena e inicia la tarea2 en intervalos de 1/2 segundo");
    }
}

void funcionTarea2() {
    Serial.print("tarea2: ");
    Serial.println(millis());
}

void funcionTarea3() {
    Serial.print("tarea3: ");
    Serial.println(millis());
  
}

void setup () {
  Serial.begin(115200);
  Serial.println("PRUEBA DEL ORGANIZADOR");

  // Inicia el organizador
  organizador.init();
  Serial.println("Organizador iniciado");

  // Añade la tarea1 y la tarea2
  organizador.addTask(tarea1);
  Serial.println("tarea1 añadida");
  organizador.addTask(tarea2);
  Serial.println("tarea2 añadida");

  delay(5000);
  // Activa las tareas
  tarea1.enable();
  Serial.println("tarea1 activada");
  tarea2.enable();
  Serial.println("tarea2 activada");
}

void loop () {
  organizador.execute();
}
