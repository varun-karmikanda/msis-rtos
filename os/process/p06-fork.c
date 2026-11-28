#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
    pid_t pid = fork();

    if(pid < 0){
        perror("Fork failed!");
        return 1;
    } else if(pid == 0){
        printf("Child: child = %d\tparent = %d\n", getpid(), getppid());
        fflush(stdout);

        execl("./p06-exec", "p06-exec", NULL);

        perror("execl failed\n");
        return 1;
    } else {
        int status;
        waitpid(pid, &status, 0);
        printf("\nParent: fork_execl pid = %d exited\n", pid);
    }
    return 0;
}