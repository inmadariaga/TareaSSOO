#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include "structs.h"

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
    return proceso;
  } else {
    // Caso Manager
    proceso->timeout = atoi(process[1]);
    proceso->children_number = atoi(process[2]);
    int* children_list = malloc(proceso->children_number * sizeof(int));
    for (int child = 3; child <= proceso->children_number + 2; child++){
      children_list[child - 3] = atoi(process[child]);
    }
    proceso->children_list = children_list;
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
    pid_t* children_pids = malloc(process->children_number * sizeof(pid_t));
    int child_id;
    pid_t child_pid;
    for (int i = 0; i <= process->children_number; i++){
      child_id = process->children_list[i];
      Process* child_process = all_process[child_id];
      if (strcmp(child_process->type, "W") == 0){
        time_t begin;
        time(&begin);
        child_process->time = begin;
      }
      child_pid = execute_process(child_id, all_process);
      children_pids[i] = child_pid;
    }
    process->children_pids = children_pids;
    for (int i = 0; i <= process->children_number; i++){
      pid_t child_pid = process->children_pids[i];
      int status;
      waitpid(child_pid, &status, WIFEXITED(status));
      Process* child_process = all_process[process->children_list[i]];
      if (strcmp(child_process->type, "W") == 0){
        time_t begin, end;
        time(&end);
        begin = child_process->time;
        child_process->time = end - begin;
      }
    }
  } else {
    // programa main
    time_t begin, end;
    time(&begin);
    pid_t cop;
    cop = fork();
    if (cop == 0){
      sleep(1);
      time(&end);
      while (end - begin < process->timeout)
      {
        sleep(1);
      }
      pid_t ppid = getppid();
      kill(ppid, SIGABRT);
      exit(0);
    } else {
      signal(SIGABRT, manager_receptor);
      // signal(SIGINT, SIG_IGN); esto es para el manager
      int status;
      pid_t exited_child = wait(&status);
    }
  }
}

pid_t execute_manager(Process* process, Process** all_process){
  pid_t pid = fork();
  if (pid == 0){
    //proceso manager
    pid_t* children_pids = malloc(process->children_number * sizeof(pid_t));
    int child_id;
    pid_t child_pid;
    for (int i = 0; i <= process->children_number; i++){
      child_id = process->children_list[i];
      Process* child_process = all_process[child_id];
      if (strcmp(child_process->type, "W") == 0){
        time_t begin;
        time(&begin);
        child_process->time = begin;
      }
      child_pid = execute_process(child_id, all_process);
      children_pids[i] = child_pid;
    }
    process->children_pids = children_pids;
    for (int i = 0; i <= process->children_number; i++)
    {
      pid_t child_pid = process->children_pids[i];
      int status;
      waitpid(child_pid, &status, WIFEXITED(status));
      Process* child_process = all_process[process->children_list[i]];
      if (strcmp(child_process->type, "W") == 0){
        time_t begin, end;
        time(&end);
        begin = child_process->time;
        child_process->time = end - begin;
      }
      char* text = output(child_process, all_process);
      child_process->text = text;
    }
    char* text = output(process, all_process);
    process->text = text;
    exit(0);
  } else {
    // proceso padre del manager 
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
    return execute_worker(process);
  } else if (strcmp(process->type, "M")){
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
