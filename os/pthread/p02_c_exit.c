#include <pthread.h>
#include <stdio.h>

static void helper(void){
    pthread_exit((void*) 67L);
}

static void *worker(void *arg){
    (void) arg;

    printf("\nthread calling helper");
    helper();

    printf("\nworker: you won't see this");
    return NULL;
}

int main(void){
    pthread_t tid;
    void *ret;

    pthread_create(&tid, NULL, worker, NULL);
    pthread_join(tid, &ret);

    printf("\nmain: thread exited with %ld", (long) ret);

    return 0;
}