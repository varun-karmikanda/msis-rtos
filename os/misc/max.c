#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
    pid_t pid_1 = fork();

    if(pid_1 == 0){
        printf("Child 1: Hello World | PID %d\n", getpid());

        execl("")
    }
}