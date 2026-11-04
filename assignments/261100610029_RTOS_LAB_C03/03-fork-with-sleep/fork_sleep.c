#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        // Child process
        printf("Child: my pid = %d, parent pid = %d\n", getpid(), getppid());
        printf("Child: sleeping for 20 seconds...\n");
        fflush(stdout);
        sleep(20);
        printf("Child: woke up, exiting now\n");
    } else {
        // Parent process
        printf("Parent: my pid = %d, child pid = %d\n", getpid(), pid);
        printf("Parent: waiting for child to finish...\n");
        fflush(stdout);

        int status;
        pid_t exited_pid = waitpid(pid, &status, 0);  // blocks here for ~20 sec

        if (exited_pid == pid) {
            if (WIFEXITED(status))
                printf("Parent: child %d exited with status %d\n", exited_pid, WEXITSTATUS(status));
            else
                printf("Parent: child %d did not exit normally\n", exited_pid);
        }
    }

    return 0;
}
