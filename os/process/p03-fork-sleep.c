#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
    pid_t pid = fork();

    if(pid < 0){
        printf("Fork exited!\n");
        return 1;
    } else if(pid == 0){
        printf("Child: child = %d\tparent = %d\n", getpid(), getppid());
        printf("Child: sleeping for 3 seconds...\n");
        fflush(stdout);
        sleep(3);
        printf("Child: woke up, exiting now!\n");
    } else {
        printf("Parent: parent = %d\tchild = %d\n", getpid(), pid);
        printf("Parent: waiting for child!\n");
        fflush(stdout);
        
        int status;
        pid_t exited_pid = waitpid(pid, &status, 0);

        if(exited_pid == pid){
            if(WIFEXITED(status)){
                printf("Parent: child %d exited normally with status %d\n", exited_pid, WEXITSTATUS(status));
            } else {
                printf("Parent: child %d exited abnormally\n", exited_pid);
            }
        }
    }
    return 0;
}
