#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("fork_exec: parent pid = %d\n", getpid());
    fflush(stdout);

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    } else if (pid == 0) {
        // Child process: about to become exec_prog
        printf("fork_exec: child pid = %d, about to exec\n", getpid());
        fflush(stdout);

        execl("./exec_prog", "exec_prog", NULL);

        // only reached if execl fails
        perror("execl failed");
        return 1;
    } else {
        // Parent waits for the child (now running as exec_prog)
        int status;
        waitpid(pid, &status, 0);
        printf("fork_exec: child (pid %d) exited\n", pid);
    }

    return 0;
}
