#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        // Child process
        // int *ptr = NULL;
        // *ptr = 67;
        printf("Child: my pid = %d, parent pid = %d\n", getpid(), getppid());
    } else {
        // Parent process
        printf("Parent: my pid = %d, child pid = %d\n", getpid(), pid);

        int status;
        pid_t exited_pid = waitpid(pid, &status, 0);  // wait specifically for this child's pid
        // pid_t exited_pid = waitpid(pid, &status, WNOHANG);  // wait specifically for this child's pid
        // pid_t exited_pid = waitpid(pid, &status, WUNTRACED);  // wait specifically for this child's pid

        if (exited_pid == pid) {
            if (WIFEXITED(status))
                printf("Parent: child %d exited with status %d\n", exited_pid, WEXITSTATUS(status));
            else
                printf("Parent: child %d did not exit normally due to SIgnal flag: %d\n", exited_pid, WTERMSIG(status));
        }
    }

    return 0;
}
