// Tells the compiler to compile this file once
#pragma once

// Define the struct
typedef struct worker {
  char* program;
  char** args;
  int execution_time;
  int return_code;
  int interrupter;
} Worker;

typedef struct manager {
  char* type;
  int timeout;
} Manager;

void execute_process(char** process, char*** all_process);
Worker* execute_worker(char** process, int arcg);
Manager* execute_manager(char** process, int argc, char*** all_process);