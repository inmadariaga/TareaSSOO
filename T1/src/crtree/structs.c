#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

// Función para ejecutar un proceso manager
Worker* execute_worker(char** process, int argc) {
  // No hice nada sobre el manejo de señales
  Worker* worker = malloc(sizeof(Worker));
  // Guardamos algunos datos
  worker->program = process[1];
  // Aqui guardamos los argumentos del proceso
  char** args[argc+1];
  for (int i = 0; i < argc+1; i++)
  { if (i == argc) {
    args[i] = NULL;
  }
  else {
    args[i] = process[i+2];
  }
    
  }
  worker->args = args;
  pid_t pid;
  pid = fork();
  if (pid == 0)
  {
    // Este es el Hijo. Aqui ejecutamos el programa del worker
    execvp(worker->program, worker->args);
  }
  else {
    // Este es el padre. Aqui hay que guardar el tiempo de ejecución, el status y si fue interrumpido.
    // Estos datos hay que guardarlos en los atributos correspondientes del struct.
  }
  // retornamos el worker
  return worker;
}

//Función para ejecutar un proceso manager
Manager* execute_manager(char** process, int argc, char*** all_process) {
  Manager* manager = malloc(sizeof(Manager));
  manager->type = process[0];
  manager->timeout = process[1];
  for (int i = 0; i < argc; i++)
  { 
    char process_id = process[i+2];
    execute_process(all_process[(int)process_id], all_process);
  }
  
}

//Executa un proceso cualquiera
void execute_process(char** process, char*** all_process) {
  int len = sizeof(process)/sizeof(char);
  
  if (process[0] == "W")
  {
    execute_worker(process, len-2);
  }
  else if (process[0] == "R" || process[0] == "M" ) {
    execute_manager(process, len-2, all_process);
  }
}