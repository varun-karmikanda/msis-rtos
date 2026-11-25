#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static int shared = 67;

static void *worker(void *arg){
    (void) arg;

    int private = 911;

    pid_t pid = getpid();
    pthread_t tid = pthread_self();

    printf("\n\npid = %d\nthread = %lu", pid, tid);
    printf("\nshared = %p\nprivate = %p", (void*) &shared, (void*) &private);


    return NULL;
}

int main(void){

    pthread_t t1, t2;

    pthread_create(&t1, NULL, worker, NULL);
    pthread_create(&t2, NULL, worker, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}