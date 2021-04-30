#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "structs.h"

extern int largo;
extern Process** todos_procesos;
//Función para crear un proceso
Process* create_process(int id, char** process, char*** all_process) {
  Process* proceso = malloc(sizeof(Process));
  proceso->id = id;
  proceso->type = process[0];
  if (strcmp(process[0],"W") == 0){
    // Caso Worker
    proceso->program = process[1];
    int args_number = atoi(process[2]); //numero de args
    char* arguments[args_number + 1];
    for (int i = 0; i <= args_number; i++){
      if (i == args_number){
        arguments[i] = NULL;
      } else {
        arguments[i] = process[3 + i];
      }
    }
    proceso->n_args = args_number;
    proceso->args = arguments;
    proceso->interrumped = 0;
    proceso->code = -1;
    return proceso;
  } else {
    // Caso Manager y root
    proceso->timeout = atoi(process[1]);
    proceso->children_number = atoi(process[2]);
    int* children_list = malloc(proceso->children_number * sizeof(int));
    for (int child = 3; child <= proceso->children_number + 2; child++){
      children_list[child - 3] = atoi(process[child]);
    }
    proceso->children_list = children_list;
    proceso->interrumped = 0;
    proceso->code = -1;
    return proceso;
  }
}

char* worker_text(Process* process){
  char* text = process->program;
  strcat(text,",");
  if (process->n_args != 0){
    for (int i = 0; i < process->n_args; i++){
    strcat(text,process->args[i]);
    strcat(text,",");
    }
  }
  char time_to_str[10];
  sprintf(time_to_str, "%ld", process->time);
  strcat(text, time_to_str);
  strcat(text,",");
  char code_to_str[3];
  sprintf(code_to_str, "%d", process->code);
  strcat(text, code_to_str);
  strcat(text,",");
  char interrumped_to_str[2];
  sprintf(interrumped_to_str, "%d", process->interrumped);
  strcat(text, interrumped_to_str);
  strcat(text, "\n");
  return text;
}

char* output(Process* process, Process** all_process){
  if (strcmp(process->type,"W") == 0){
    return worker_text(process);
  } else {
    char* text;
    for (int i = 0; i < process->children_number; i++){
      Process* child_process = all_process[process->children_list[i]];
      strcat(text, child_process->text);
    }
    return text;
  }
}

pid_t execute_root(Process* process, Process** all_process){
  pid_t pid = fork();
  if (pid == 0){
    // proceso root
    signal(SIGABRT, kill_handler);
    signal(SIGINT, kill_handler);
    pid_t* children_pids = malloc(process->children_number * sizeof(pid_t));
    int child_id;
    pid_t child_pid;
    Process* child_process;
    for (int i = 0; i < process->children_number; i++){
      child_id = process->children_list[i];
      child_process = all_process[child_id];
      time_t begin;
      time(&begin);
      child_process->time = begin;
      child_pid = execute_process(child_id, all_process);
      child_process->pid = child_pid;
      printf("ROOT PID %d", process->pid);
      printf("ITERACION %d\n", i);
      printf("%s %d\n",child_process->type, process->id);
      printf("CHILDPID %d\n", child_pid);
      children_pids[i] = child_pid;
    }
    process->children_pids = children_pids;
    int status;
    pid_t exited;
    int exited_childs = 0;
    time_t begin, end;
    while (exited_childs < process->children_number)
    {
      for (int i = 0; i < process->children_number; i++){
        pid_t child_pid = process->children_pids[i];
        exited = waitpid(child_pid, &status, WNOHANG);
        child_process = all_process[process->children_list[i]];
        if (exited != 0 && exited != -1){
          if (strcmp(child_process->type, "W") == 0){
            time(&end);
            begin = child_process->time;
            child_process->time = end - begin;
            child_process->code = WEXITSTATUS(status);
          }
          exited_childs++;
        } else if (exited != -1 && child_process->interrumped == 0) {
          if (strcmp(child_process->type, "M") == 0){
            time(&end);
            if (end - child_process->time < child_process->timeout) {
              exited_childs++;
              interrupt_process(child_process, all_process); //modifica datos almacenados para asi poder g
              kill(child_pid, SIGABRT);
            }
          }
        }
      }
      exit(0);
    }
  } else {
    // programa main
    int status;
    pid_t exited;
    time_t begin, end;
    time(&begin);
    time(&end);
    while (end - begin < process->timeout)
    {
      exited = waitpid(pid, &status, WNOHANG);
      if (exited != 0) {
        return 0;
      }
      sleep(1);
      time(&end);
    }
    process->interrumped = 1;
    kill(pid, SIGABRT);
    return 0;
  }
}

void interrupt_process(Process* process, Process** all_process){
  process->interrumped = 1;
  if (strcmp(process->type, "M") == 0){
    Process* child;
    for (int i = 0; i < process->children_number; i++){
      if (child->code == -1 && child->interrumped == 0){
        child = all_process[process->children_list[process->children_number]];
        interrupt_process(child, all_process);
      }
    }
  }
}

pid_t execute_manager(Process* process, Process** all_process){
  pid_t pid = fork();
  if (pid == 0){
    // proceso manager
    signal(SIGABRT, kill_handler);
    signal(SIGINT, SIG_IGN);
    pid_t* children_pids = malloc(process->children_number * sizeof(pid_t));
    int child_id;
    pid_t child_pid;
    Process* child_process;
    for (int i = 0; i < process->children_number; i++){
      child_id = process->children_list[i];
      child_process = all_process[child_id];
      time_t begin;
      time(&begin);
      child_process->time = begin;
      child_pid = execute_process(child_id, all_process);
      printf("CHILD PID %d\n", child_pid);
      child_process->pid = child_pid;
      children_pids[i] = child_pid;
    }
    process->children_pids = children_pids;
    int status;
    pid_t exited;
    int exited_childs = 0;
    time_t begin, end;
    while (exited_childs < process->children_number)
    {
      for (int i = 0; i < process->children_number; i++){
        pid_t child_pid = process->children_pids[i];
        exited = waitpid(child_pid, &status, WNOHANG);
        child_process = all_process[process->children_list[i]];
        if (exited != 0 && exited != -1){
          if (strcmp(child_process->type, "W") == 0){
            time(&end);
            begin = child_process->time;
            child_process->time = end - begin;
            child_process->code = WEXITSTATUS(status);
          }
          exited_childs++;
        } else if (exited != -1 && child_process->interrumped == 0) {
          if (strcmp(child_process->type, "M") == 0){
            time(&end);
            if (end - child_process->time < child_process->timeout) {
              exited_childs++;
              interrupt_process(child_process, all_process);
              kill(child_pid, SIGABRT);
            }
          }
        }
      }
      exit(0);
    }
  } else {
    return pid;
  }
}

pid_t execute_worker(Process* process){
  pid_t child_pid = fork();
  if (child_pid == 0){
    execv(process->program, process->args);
  } else {
    return child_pid;
  }
}

//ejecuta un proceso cualquiera
pid_t execute_process(int process_id, Process** all_process){
  Process* process = all_process[process_id];
  if (strcmp(process->type, "W") == 0){
    printf("WORKER %d\n", process_id);
    return execute_worker(process);
  } else if (strcmp(process->type, "M") == 0){
    printf("manager %d\n", process_id);
    return execute_manager(process, all_process);
  } else {
    return execute_root(process, all_process);
  }
}

void manager_receptor(Process* process){
  for (int i = 0; i < process->children_number; i++){
    kill(process->children_pids[i], SIGABRT);
  }
}

void kill_handler(int sig){
  printf("HAY KILL\n");
  pid_t pid = getpid();
  int id;
  Process* process;
  for (int i = 0; i < largo; i++){
    process = todos_procesos[i];
    if (process->pid == pid){
      id = process->id;
    }
  }
  process = todos_procesos[id];
  Process* child;
  for (int i = 0; i < process->children_number; i++){
    child = process->children_list[i];
    kill(child->pid, SIGABRT);
  }
}

//Funcion para liberar un proceso manager
void free_process(Process* process){
  if (strcmp(process->type, "W") == 0) {
    free(process->args);
    free(process);
  } else {
    free(process->children_list);
    free(process->children_pids);
    free(process);
  }
}
