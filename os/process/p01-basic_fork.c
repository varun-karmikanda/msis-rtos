#include <stdio.h>
#include  <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
    pid_t pid = fork();

    if(pid < 0){
        printf("Fork failed!\n");
        return 1;
    } else if(pid == 0){
        printf("Child: pid = %d\t parent pid = %d\n", getpid(), getppid());
    } else {
        printf("Parent: pid = %d\t child pid = %d\n", getpid(), pid);
        wait(NULL);
        printf("Parent: child exited!\n");
    }
    return 0;
}