#include <pthread.h>
#include <stdio.h>

#define N 3

static void *worker(void *arg){
    int id = *(int*) arg;

    printf("\nworker: sees id %d", id);

    return NULL;
}

int main(void){
    pthread_t tid[N];

    for (int i = 0; i < N; i++){
        pthread_create(&tid[i], NULL, worker, &i);
    }

    for (int i = 0; i < N; i++){
        pthread_join(tid[i], NULL);
    }
    
    return 0;
}