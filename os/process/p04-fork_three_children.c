#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N 3

int main(void){
    int sleep_times[N] = {4, 6, 7};
    pid_t child_pid[N];

    for (int i = 0; i < N; i++)
    {
        pid_t pid = fork();

        if(pid < 0){
            printf("Fork failed!\n");
            return 1;
        } else if (pid == 0)
        {
            printf("Child %d: child = %d\tparent = %d\n", i, getpid(), getppid());
            printf("Child %d: sleeping for %d seconds...\n", i, sleep_times[i]);
            fflush(stdout);
            sleep(sleep_times[i]);
            printf("child %d: woke up and exited!\n", i);
            return 0;
        } else {
            child_pid[i] = pid;
        }
    }

    printf("Parent: parent = %d waiting for all child\n", getpid());
    fflush(stdout);

    for (int i = 0; i < N; i++)
    {
        int status;
        pid_t exited_pid = waitpid(child_pid[i], &status, 0);

        if(exited_pid == child_pid[i] && WIFEXITED(status)){
            printf("Parent: child %d pid = %d exited normally with %d\n", i, exited_pid, WEXITSTATUS(status));
        }
    }
    
    printf("Parent: all children exited!\n");
    return 0;
}