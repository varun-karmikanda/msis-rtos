
#include <stdio.h>
#include <unistd.h>

int main() {
    printf("Hello World\n");
    printf("PID: %d, PPID: %d\n", getpid(), getppid());

    sleep(120);

    return 0;
}
