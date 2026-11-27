#include <stdio.h>
#include <unistd.h>

#define N 10

int main(void){
    printf("Exec: pid = %d\tstarted counting...\n", getpid());

    for (int i = 0; i < N; i++)
    {
        printf("%d\t", i);
        fflush(stdout);
        sleep(1);
    }

    return 0;    
}