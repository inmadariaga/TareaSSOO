#pragma once


typedef struct Process {
  int pid;
  int queue;
  char* name;
  int priority;
  int quantum;
  int state; // 0=running,1=ready,2=waiting,3=finished,-1 proceso no ha llegado
  int runs; // el número de veces que corrio en CPU
  int interrupted; // número de veces que fue interrumpido
  int t_arrival; //instante de llegada
  int t_finish; //instante en que termino de ejecutar
  int t_executed; //instante en que fue ejecutado por primera vez
  int waiting_time; // timepo en que esta en ready o waiting
  int cycles;
  int wait;
  int waiting_delay;
  int wait_status;
  int waiting_delay_status;
  int n_pops;
} Process;

typedef struct Queue {
  int size, capacity, priority, quantum, id;
  int* arr; //cola de procesos
} Queue;

void enqueue(Queue* queue, int process, Process** processes);
int dequeue(Queue* queue, int index);

Process* init_process(int pid, char* name, int t_arrival,int cycles,int wait, int waiting_delay);
Queue* init_queue(int capacity, int priority, int quantum, int id);
void check_arrival(Process* process, Queue* queue, int time, int index, Process** processes);
void handle_waiting(Process* process);
void check_waitings(Process** processes, int n_processes);
void print_arr(int* arr, int len);
