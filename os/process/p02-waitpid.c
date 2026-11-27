#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
    pid_t pid = fork();

    if(pid < 0){
        printf("Fork failed!\n");
        return 1;
    } else if(pid == 0){
        printf("Child: child = %d\tparent = %d\n", getpid(), getppid());
        // int *ptr = NULL;
        // *ptr = 67;
        // abort();
    } else {
        printf("Parent: parent = %d\tchild = %d\n", getpid(), pid);

        int status;
        pid_t exited_pid = waitpid(pid, &status, 0);

        if(exited_pid == pid){
            if(WIFEXITED(status)){
                printf("Parent: Child %d exited with status %d", exited_pid, WEXITSTATUS(status));
            } else {
                printf("Parent: Child %d did not exit normally", exited_pid);
            }
        }
    }
    return 0;
}