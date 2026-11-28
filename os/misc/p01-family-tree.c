#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#define N 3
#define BRANCH 2

void create_tree(int depth){
    printf("PID = %d\tPPID = %d\tDEPTH = %d\n", getpid(), getppid(), depth);

    if(depth == N) return;

    for (int i = 0; i < BRANCH; i++)
    {
        pid_t pid = fork();

        if(pid == 0){
            sleep(2);
            create_tree(depth + 1);
            exit(0);
        }
    }

    while (wait(NULL) > 0);
    
    
}

int main(void){
    create_tree(0);
    return 0;
}