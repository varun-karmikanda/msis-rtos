/* p4b_mutex.c - protect shared data with a mutex.
   Build: gcc -Wall -pthread -o p4b_mutex p4b_mutex.c */
#include <pthread.h>
#include <stdio.h>

#define N_THREADS 4
#define N_ITERS   1000000

static long counter = 0;

/* A mutex that is ready to use, set up at compile time. */
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static void *add(void *arg)
{
    (void)arg;
    for (int i = 0; i < N_ITERS; i++) {
        /* Only one thread at a time gets past this line. */
        pthread_mutex_lock(&lock);
        counter++;
        /* Let the next waiting thread in. */
        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

int main(void)
{
    pthread_t tid[N_THREADS];

    for (int i = 0; i < N_THREADS; i++)
        pthread_create(&tid[i], NULL, add, NULL);
    for (int i = 0; i < N_THREADS; i++)
        pthread_join(tid[i], NULL);

    printf("expected %d, got %ld\n", N_THREADS * N_ITERS, counter);
    return 0;
}