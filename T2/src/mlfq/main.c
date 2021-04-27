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
    printf("T=%i\n", t);
    printf("current_process:%i\n", current_process);
    // operaciones para todos los procesos que no son el actual
    for (int i = 0; i < n_processes; i++) {
      Process* process = processes[i];
      // si no ha llegado verifico si tiene que llegar
      if (process->state == -1) {
        check_arrival(process, queues[0], t, i+1, processes);
      }

    }
    printf("Cola 0-->");
    print_arr(queues[0]->arr, n_processes);
    printf("Cola 1-->");
    print_arr(queues[1]->arr, n_processes);
    if (current_process) {

      Process* process = processes[current_process-1];
      Queue* current_queue = queues[process->queue];
      int runs = process->runs;
      check_waitings(processes, n_processes);

      // ¿Se le acabo el quantum?
      if (!(runs%process->quantum)) {
        printf("Proceso %i perdio quantum\n", current_process);
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
      else if (!process->wait_status && process->wait) {
        process->state = 2;
        enqueue(queues[0], current_process, processes);
        current_process = 0;
      }
      else {
        // hay que ejecutar
        if (process->wait) {
          process->wait_status -= 1;
        }
        process->runs += 1;
        process->cycles -= 1;
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
      int find_process = 0;
      for (int i = 0; i < n_queues; i++)
      {
        Queue* queue = queues[i];
        int process_id = 0;
        printf("Cola:%i, size-->%i\n", i, queue->size);
        if (queue->size > 0) {
          Process* process;
          for (int j = 0; j < queue->size; j++) {
            //printf("la madre\n");
            process_id = queue->arr[j];
            if (process_id) {
              process = processes[process_id-1];

              if (process->state == 1) {
                current_process = dequeue(queue, j);
                printf("Saco a %i\n", current_process);
                find_process = 1;
                process->state = 0;
                process->n_pops +=1;
                if (!process->runs) {
                  process->t_executed = t;
                }
                if (process->wait) {
                  process->wait_status -=1;
                }
                process->runs += 1;
                process->cycles -= 1;
                if (!process->cycles) {
                    process->state = 3;
                    process->t_finish = t;
                    finish +=1;
                    current_process = 0;
                  }
                break;
              }
            }
          }

        }
        if (find_process) {
          break;
        }
      }
      check_waitings(processes, n_processes);

    }

    // ejecutamos S
    if (!(t%S)) {
      for (int i = 1; i < n_queues; i++)
      {
        Queue* queue = queues[i];
        int size = queue->size;
        if (size > 0) {
          for (int j = 0; j < size; j++)
          {
            int process_id = dequeue(queue, 0);
            enqueue(queues[0], process_id, processes);
          }

        }
      }

    }
    t+=1;
    printf("----------------\n");
    


  }
  FILE* output = fopen(output_file, "w");
  for (int i = 0; i < n_processes; i++)
  {
    Process* process = processes[i];
    int tournarround = process->t_finish - process->t_arrival;
    int response_time = process->t_executed - process->t_arrival;
    fprintf(output,"%s,%i,%i,%i,%i,%i\n",process->name, process->n_pops, process->interrupted, tournarround, response_time, process->waiting_time);

  }
  fclose(output);
  free(processes);
  free(queues);
  input_file_destroy(f);
}
