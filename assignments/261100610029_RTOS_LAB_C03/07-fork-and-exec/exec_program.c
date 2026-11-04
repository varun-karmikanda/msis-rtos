#include <stdio.h>
#include <unistd.h>

int main() {
    printf("exec_prog: pid = %d, starting count\n", getpid());
    fflush(stdout);

    for (int i = 1; i <= 10; i++) {
        printf("%d\n", i);
        fflush(stdout);
        sleep(1);  // slows it down so you have time to inspect it while running
    }

    return 0;
}
