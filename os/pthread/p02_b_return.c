#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static void *square(void *arg){
    int n = *(int*) arg;

    int *result = malloc(sizeof(int));
    *result = n * n;

    return result;
}

int main(void){
    pthread_t tid;
    int n = 67;
    void *ret;

    pthread_create(&tid, NULL, square, &n);
    pthread_join(tid, &ret);

    printf("square of 67 = %d", *(int*) ret);
    free(ret);

    return 0;
}