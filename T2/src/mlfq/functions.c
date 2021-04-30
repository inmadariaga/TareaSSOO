#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

void check_arrival(Process* process, Queue* queue, int time, int index, Process** processes){
  //si llega lo encolo en la primera cola
  if (process->t_arrival == time) {
    process->state = 1;
    process->queue = queue->id;
    enqueue(queue, index, processes);
  }
}
void handle_waiting(Process* process) {
  // si el proceso esta esperando

  if (process->state == 2)
  {
    process->waiting_time +=1;
    process->waiting_delay_status -= 1;
    // ¿Lo tengo que despertar?
    if (!process->waiting_delay_status) {
      process->state = 1; // lo despierto
      process->wait_status = process->wait; // reseteo el wait_status
      process->waiting_delay_status = process->waiting_delay; // reseteo el waiting delay
    }

  }
  // si el proceso esta ready
  else if (process->state == 1) {
    process->waiting_time +=1;
  }
}

void check_waitings(Process** processes, int n_processes) {
  for (int i = 0; i < n_processes; i++) {
      handle_waiting(processes[i]);
  }
}
