#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SLEEP_TIME 3

void spawn_chain(int current, int max_depth){
    if(current > max_depth) return;

    pid_t pid = fork();

    if(pid < 0){
        perror("Fork failed!\n");
        return;
    } else if(pid ==0) {
        printf("Child %d: child = %d\t parent = %d\n", current, getpid(), getppid());

        spawn_chain(current + 1, max_depth);

        printf("Child %d: sleeping for %d seconds...\n", current, SLEEP_TIME);

        sleep(SLEEP_TIME);
        printf("Child %d: pid = %d woke up, exiting...\n", current, getpid());

        exit(0);
    } else {
        int status;
        waitpid(pid, &status, 0);

        if(current == 1){
            printf("Original Parent: parent = %d\tchild 1: pid = %d exited\n", getpid(), pid);
        } else {
            printf("Child %d: child = %d\t child %d: child = %d\n", current - 1, getpid(), current, pid);
        }
    }
}

int main(void){
    printf("Original parent: pid %d\n", getpid());

    spawn_chain(1, 67);

    return 0;
}