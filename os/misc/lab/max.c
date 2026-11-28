#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void){
    pid_t pid_1 = fork();

    if(pid_1 == 0){
        sleep(10);
        printf("Child 1: Hello World | PID = %d\n", getpid());

        execl("hello_world", "hello_world", NULL);

        perror("Child 1: Hello World | exec failed\n");
        return 1;
    }

    pid_t pid_2 = fork();

    if(pid_2 == 0){
        sleep(10);
        printf("Child 2: Current directory | PID = %d\n", getpid());

        execlp("pwd", "pwd", NULL);

        perror("Child 2: Current directory | exec failed\n");
        return 1;
    }

    pid_t pid_3 = fork();

    if(pid_3 == 0){
        sleep(10);
        printf("Child 3: Fork 2 childrens | PID = %d\n", getpid());

        pid_t pid_3_1 = fork();
        if(pid_3_1 == 0){
            sleep(10);
            printf("Child 3_1: hello from grand child 1 | PID = %d\n", getpid());
            
            pid_t pid_3_1_1 = fork();
            if(pid_3_1_1 == 0){
                sleep(10);
                printf("Child 3_1_1: hello from great grand child 1 | PID = %d\n", getpid());
                exit(0);
            }

            exit(0);
        }

        pid_t pid_3_2 = fork();
        if(pid_3_2 == 0){
            sleep(10);
            printf("Child 3_2: hello from grand child 2 | PID = %d\n", getpid());
            exit(0);
        }
    }

    while(wait(NULL) > 0);
    sleep(2);
    char cmd[64];
    snprintf(cmd, sizeof cmd, "pstree -p %d", getpid());
    system(cmd);
}