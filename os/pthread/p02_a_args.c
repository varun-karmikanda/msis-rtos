#include <pthread.h>
#include <stdio.h>

#define N 3

static void *worker(void *arg){
    int id = *(int*) arg;

    printf("\nWorker got id: %d", id);

    return NULL;
}

int main(void){

    pthread_t tid[N];

    int ids[N];

    for (int i = 0; i < N; i++){
        ids[i] = i;

        pthread_create(&tid[i], NULL, worker, &ids[i]);
    }

    for (int i = 0; i < N; i++){
        pthread_join(tid[i], NULL);
    }    

    return 0;
}