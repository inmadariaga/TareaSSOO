#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

void enqueue(Queue* queue, int process, Process** processes) {
  queue->last = (queue->last + 1) % queue->capacity;
  queue->arr[queue->last] = process;
  queue->size += 1;
  processes[process-1]->priority = queue->priority;
  processes[process-1]->quantum = queue->quantum;
  processes[process-1]->queue = queue->id;  
}

int dequeue(Queue* queue) {
  int head = queue->head;  
  int process = queue->arr[head];
  queue->head = (queue->head + 1) % queue->capacity;
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
  return process;
}

Queue* init_queue(int capacity, int priority, int quantum, int id) {
  Queue* queue = malloc(sizeof(Queue));
  int* arr = malloc(capacity*sizeof(int));
  queue->size = 0;
  queue->last = capacity - 1;
  queue->capacity = capacity;
  queue->head = 0;
  queue->arr = arr;
  queue->priority = priority;
  queue->quantum = quantum;
  queue->id = id;
  return queue;
}

int head(Queue* queue) {
  return queue->arr[queue->head];
}


