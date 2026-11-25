#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static void *background(void *arg){
    (void) arg;

    sleep(1);
    printf("\nbackground: done, resources freed successfully");

    return NULL;
}

int main(void){
    pthread_t tid;

    pthread_create(&tid, NULL, background, NULL);
    pthread_detach(tid);

    printf("\nmain: not waiting for thread");
    sleep(2);

    return 0;
}