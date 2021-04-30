#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

int main(int argc, char **argv)
{
    time_t begin, end;
    int max_wait = 10;
    pid_t pid;
    pid = fork();
    int exited;
    if (pid == 0){
        sleep(7);
        int i = 1 + 1;
        exit(i);
    } else {
        time(&begin);
        time(&end);
        int status;
        while (end - begin <= max_wait)
        {
            printf("TIME %ld\n", end - begin);
            exited = waitpid(pid, &status, WNOHANG);
            printf("%d STATUS %d \n", exited ,WEXITSTATUS(status));
            if (exited != 0){
                printf("termino el hijo\n");
                return 0;
            }
            sleep(1);
            time(&end);
        }
        if (exited == 0){
            printf("PADRE VA A MATAR AL HIJO\n");
            kill(pid, SIGABRT);
            exited = waitpid(pid, &status, WNOHANG);
            printf("PID %d\nstatus %d\n",exited,status);
        }
    }
}