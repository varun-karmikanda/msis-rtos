#include<pthread.h>
#include<stdio.h>

static void *hello(){
    printf("\nHelooo from thread");
    return NULL;
} 

int main(void){
    pthread_t pid;

    pthread_create(&pid, NULL, hello, NULL);
    pthread_join(pid, NULL);
    printf("\nHello");

    printf("\npid after %lu", pid);

    return 0;
}