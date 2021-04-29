// Tells the compiler to compile this file once
#pragma once

// Define the struct
typedef struct process {
  // transversal a ambos
  char* type;
  int id;
  // worker
  char* program;
  int n_args;
  char** args;
  time_t time;
  int code;
  int interrumped;
  // manager
  int timeout;
  int children_number;
  int* children_list;
  pid_t* children_pids;
  // retorno
  char text;
} Process;

Process* create_process(int id, char** process, char*** all_process);
pid_t execute_process(int process_id, Process** all_process);
void manager_receptor(Process* process);
void free_process(Process* process);