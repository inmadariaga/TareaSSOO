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
} Process;

typedef struct Queue {
  int size,last, capacity, head, priority, quantum, id;
  int* arr; //cola de procesos
} Queue;

void enqueue(Queue* queue, int process, Process** processes);
int dequeue(Queue* queue);

Process* init_process(int pid, char* name, int t_arrival,int cycles,int wait, int waiting_delay);
Queue* init_queue(int capacity, int priority, int quantum, int id);
int head(Queue* queue);
void check_arrival(Process* process, Queue* queue, int time, int index, Process** processes);
void handle_waiting(Process* process);
