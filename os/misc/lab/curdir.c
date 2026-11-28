#include <stdio.h>
#include <unistd.h>
#include <limits.h>

int main(void){
    char cwd[PATH_MAX];

    getcwd(cwd, sizeof(cwd));

    printf("Current Directory: %s\n", cwd);
    printf("PID: %d, PPID: %d\n", getpid(), getppid()); 
}