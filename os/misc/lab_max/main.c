
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {

    // setbuf(stdout, NULL);

    printf("Parent Process\n");
    printf("PID: %d, PPID: %d\n\n", getpid(), getppid());

    // CHILD 1
    pid_t c1 = fork();

    if (c1 == 0) {
        printf("Child 1 executing hello program\n");
        execl("./hello", "hello", NULL);

        perror("exec hello failed");
        exit(1);
    }

    // CHILD 2
    pid_t c2 = fork();

    if (c2 == 0) {
        printf("Child 2 executing pwd program\n");
        execl("./pwd", "pwd", NULL);

        perror("exec pwd failed");
        exit(1);
    }

    // CHILD 3
    pid_t c3 = fork();

    if (c3 == 0) {

        printf("\nI'm Child 3\n");
        printf("PID: %d, PPID: %d\n", getpid(), getppid());

        // GRANDCHILD 1
        pid_t gc1 = fork();

        if (gc1 == 0) {

            printf("\nHello from Grandchild 1\n");
            printf("PID: %d, PPID: %d\n", getpid(), getppid());

            // GREAT GRANDCHILD
            pid_t ggc = fork();

            if (ggc == 0) {

                printf("\nHello from Great Grandchild\n");
                printf("PID: %d, PPID: %d\n",
                       getpid(), getppid());

                sleep(120);
                exit(0);
            }

            sleep(120);
            wait(NULL);
            exit(0);
        }

        // GRANDCHILD 2
        pid_t gc2 = fork();

        if (gc2 == 0) {

            printf("\nHello from Grandchild 2\n");
            printf("PID: %d, PPID: %d\n", getpid(), getppid());

            sleep(120);
            exit(0);
        }

        sleep(120);

        wait(NULL);
        wait(NULL);

        exit(0);
    }

    // PARENT WAITS FOR ALL THREE CHILDREN
    sleep(120);

    wait(NULL);
    wait(NULL);
    wait(NULL);

    printf("\nAll processes completed.\n");

    return 0;
}
