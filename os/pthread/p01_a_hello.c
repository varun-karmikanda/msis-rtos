#include <pthread.h>
#include <stdio.h>

static void *hello(void *arg){
    (void) arg;

    printf("\nHello from new thread!");

    return NULL;
}

int main(void){
    pthread_t tid;

    pthread_create(&tid, NULL, hello, NULL);

    pthread_join(tid, NULL);

    printf("\nThread finished, main exited!");

    return 0;
}