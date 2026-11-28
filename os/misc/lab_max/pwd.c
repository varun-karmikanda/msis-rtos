
#include <stdio.h>
#include <unistd.h>
#include <limits.h>

int main() {
    char cwd[PATH_MAX];

    getcwd(cwd, sizeof(cwd));

    printf("Current Directory: %s\n", cwd);
    printf("PID: %d, PPID: %d\n", getpid(), getppid());

    sleep(120);

    return 0;
}
