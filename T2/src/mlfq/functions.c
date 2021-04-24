#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

void check_arrival(Process* process, Queue* queue, int time, int index, Process** processes){
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
    // ¿Lo tengo que despertar?
    if (!process->waiting_delay_status) {
      process->state = 1; // lo despierto
      process->wait_status = process->wait; // reseteo el wait
      process->waiting_delay_status = process->waiting_delay; // reseteo el waiting delay 
    }
    else {
      process->waiting_time +=1;
      process->waiting_delay_status -= 1;
    }
  }
  // si el proceso esta ready
  else if (process->state == 1) {
    process->waiting_time +=1;
  }
  
}
