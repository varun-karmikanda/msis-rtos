#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("Original parent: pid = %d\n", getpid());
    fflush(stdout);

    pid_t pid1 = fork();

    if (pid1 < 0) {
        perror("fork failed");
        return 1;
    } else if (pid1 == 0) {
        // ---- child1 ----
        printf("Child1: pid = %d, parent pid = %d\n", getpid(), getppid());
        fflush(stdout);

        pid_t pid2 = fork();

        if (pid2 < 0) {
            perror("fork failed");
            return 1;
        } else if (pid2 == 0) {
            // ---- child2 ----
            printf("Child2: pid = %d, parent pid = %d\n", getpid(), getppid());
            fflush(stdout);

            pid_t pid3 = fork();

            if (pid3 < 0) {
                perror("fork failed");
                return 1;
            } else if (pid3 == 0) {
                // ---- child3 ----
                printf("Child3: pid = %d, parent pid = %d, sleeping 10s\n", getpid(), getppid());
                fflush(stdout);
                sleep(10);
                printf("Child3: pid = %d, woke up, exiting\n", getpid());
                return 0;
            } else {
                // child2 waits for child3
                int status;
                waitpid(pid3, &status, 0);
                printf("Child2: pid = %d, child3 (pid %d) exited\n", getpid(), pid3);
                sleep(10);
                printf("Child2: pid = %d, woke up, exiting\n", getpid());
                return 0;
            }
        } else {
            // child1 waits for child2
            int status;
            waitpid(pid2, &status, 0);
            printf("Child1: pid = %d, child2 (pid %d) exited\n", getpid(), pid2);
            sleep(10);
            printf("Child1: pid = %d, woke up, exiting\n", getpid());
            return 0;
        }
    } else {
        // ---- original parent ----
        int status;
        waitpid(pid1, &status, 0);
        printf("Original parent: pid = %d, child1 (pid %d) exited\n", getpid(), pid1);
    }

    return 0;
}
