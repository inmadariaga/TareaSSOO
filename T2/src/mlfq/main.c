#include <stdio.h>
#include <stdlib.h>
#include "../file_manager/manager.h"
#include "./structs.h"
int main(int argc, char **argv)
{
  char *input_file = argv[1];
  char *output_file = argv[2];
  int n_queues = atoi(argv[3]);
  int q = atoi(argv[4]);
  int S = atoi(argv[5]);
  InputFile* f = read_file(input_file);
  int n_processes = f->len;
  Process** processes = malloc(n_processes * sizeof(Process)); // lista de procesos
  Queue** queues = malloc(n_queues*sizeof(Queue)); //lista de colas
  //inicializando procesos
  for (int i = 0; i < n_processes; i++)
  { 
    char* name = f->lines[i][0];
    int pid = atoi(f->lines[i][1]);
    int t_arrival = atoi(f->lines[i][2]);
    int cycles = atoi(f->lines[i][3]);
    int wait = atoi(f->lines[i][4]);
    int waiting_delay = atoi(f->lines[i][5]);
   Process* process = init_process(pid,name,t_arrival,cycles,wait,waiting_delay);
   processes[i] = process;
  }
  // inicializando colas
  for (int i = 0; i < n_queues; i++)
  { int priority = n_queues-(i+1);
    int quantum = (n_queues - priority)*q;
    Queue* queue = init_queue(n_processes, priority, quantum, i);
    queues[i] = queue;
  }
  int finish = 0;
  int t = 0;
  int current_process = 0;
  while(finish < n_processes) {
    // operaciones para todos los procesos que no son el actual
    for (int i = 1; i <= n_processes; i++) {
      Process* process = processes[i-1];
      if (i != current_process) {
        // si el proceso no ha llegado verificamos si tiene que llegar
        if (process->state == -1) {
          check_arrival(process, queues[0], t, i, processes);
        }
        // si ya llego y no ha finalizado
        else if(process->state != 3){
          handle_waiting(process);
        }
        
      }
      
    }
    if (current_process) {
      Process* process = processes[current_process-1];
      Queue* current_queue = queues[process->queue];
      int runs = process->runs;     
      // ¿Se le acabo el quantum?
      if (!(runs%process->quantum)) {
        process->state = 1;
        process->interrupted +=1;
        if (process->queue == n_queues-1) {
          enqueue(current_queue, current_process, processes);
          current_process = 0;
        }
        else {
          enqueue(queues[process->queue + 1], current_process, processes);
          current_process = 0;
        }
      }
      // se le acabo el wait?
      else if (!process->wait_status) {
        process->state = 2;
        process->interrupted +=1;
        enqueue(queues[0], current_process, processes);
        current_process = 0;
      }
      else {
        // hay que ejecutar
        process->wait_status -= 1;
        process->runs +=1;
        process->cycles -=1;
        if (!process->cycles) {
            process->state = 3;
            process->t_finish = t;
            finish +=1;
            current_process = 0;
          }
      }


    }
    else {
      // sacar un proceso de alguna de las colas
      int i = 0;
      while (i < n_queues)
      {
        Queue* queue = queues[i];
        if (queue->size > 0) {
          Process* process = processes[queue->arr[queue->head]-1];
          if (process->state == 1) {
            process->state = 0;
            current_process = dequeue(queue);
            if (!process->runs) {
              process->t_executed = t;
            }
            process->runs +=1;
            process->wait_status -=1 ;
            process->cycles -=1;
            if (!process->cycles) {
                process->state = 3;
                process->t_finish = t;
                finish +=1;
                current_process = 0;
              }
            break;
          }
          
        }
        i +=1;
      }
      
    }
    
    // ejecutamos S
    if (t==S) {
      for (int i = 1; i < n_queues; i++)
      {
        Queue* queue = queues[i];
        if (queue->size > 0) {
          for (int j = 0; j < queue->size; j++)
          {
            int process_id = dequeue(queue);
            enqueue(queues[0], process_id, processes);
          }
          
        }
      }
      
    }
    t+=1;
   
    
  }
  FILE* output = fopen(output_file, "w");
  for (int i = 0; i < n_processes; i++)
  { 
    Process* process = processes[i];
    int tournarround = process->t_finish - process->t_arrival;
    int response_time = process->t_executed - process->t_arrival;
    fprintf(output,"%s,%i,%i,%i,%i,%i\n",process->name, process->runs, process->interrupted, tournarround, response_time, process->waiting_time);
   
  }
  fclose(output);
  free(processes);
  free(queues);
  input_file_destroy(f);
}
