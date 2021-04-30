#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"
#include "../file_manager/manager.h"

int largo = 0;
Process** todos_procesos;

int main(int argc, char **argv)
{ 
  printf("%d\n", getpid());
  InputFile* file = read_file(argv[1]);
  largo = 0;
  // funcion que instancia los procesos en memoria
  Process** all_process = malloc(file->len * sizeof(Process));
  todos_procesos = all_process;
  for (int i = 0; i < file->len; i++){
    Process* process = create_process(i, file->lines[i], file->lines);
    all_process[i] = process;
  }
  //FILE* output = fopen(strcat(argv[2], ".txt"), "wt"); // para escribir es fputs("TEXTO A ESCRIBIR\n", output)
  // funcion que hace la simulacion
  Process* process;
  execute_process(all_process[atoi(argv[2])]->id, all_process);
  FILE* output;
  char number[1000];
  for (int i = 0; i < file->len; i++){
    process = all_process[i];
    sprintf(number, "%d", i + 1);
    output = fopen(strcat(number, ".txt"), "wt");
    fprintf(output, "%s" ,process->text);
    fclose(output);
  }
  input_file_destroy(file);
  //funcion que libera memoria
  for (int i = 0; i < file->len; i++){
    free_process(all_process[i]);
  }
  free(all_process);
}