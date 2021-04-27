#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

void enqueue(Queue* queue, int process, Process** processes) {
  for (int i = 0; i < queue->capacity; i++) {
    if (!queue->arr[i]) {
      queue->arr[i] = process;
      break;
    }
  }
  queue->size += 1;
  processes[process-1]->priority = queue->priority;
  processes[process-1]->quantum = queue->quantum;
  processes[process-1]->queue = queue->id;
}

int dequeue(Queue* queue, int index) {
  int process = queue->arr[index];
  for (int i = index; i < queue->capacity; i++) {
    if (i == queue->capacity-1) {
      queue->arr[i] = 0;
    }
    else {
      queue->arr[i] = queue->arr[i+1];
    }

  }
  queue->size -=1;
  return process;
}

Process* init_process(int pid, char* name, int t_arrival,int cycles,int wait, int waiting_delay) {
  Process* process = malloc(sizeof(Process));
  process->pid = pid;
  process->name = name;
  process->state = -1;
  process->runs = 0;
  process->interrupted = 0;
  process->t_arrival = t_arrival;
  process->waiting_time = 0;
  process->cycles = cycles;
  process->wait = wait;
  process->waiting_delay = waiting_delay;
  process->wait_status = process->wait;
  process->waiting_delay_status = process->waiting_delay;
  process->n_pops = 0;
  return process;
}

Queue* init_queue(int capacity, int priority, int quantum, int id) {
  Queue* queue = malloc(sizeof(Queue));
  int* arr = calloc(capacity, sizeof(int));
  queue->size = 0;
  queue->capacity = capacity;
  queue->arr = arr;
  queue->priority = priority;
  queue->quantum = quantum;
  queue->id = id;
  return queue;
}
