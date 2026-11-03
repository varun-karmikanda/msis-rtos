/* p4a_race.c - threads updating shared data without a lock.
   Build: gcc -Wall -pthread -o p4a_race p4a_race.c */
#include <pthread.h>
#include <stdio.h>

#define N_THREADS 4
#define N_ITERS   1000000

/* Shared by all threads. */
static long counter = 0;

static void *add(void *arg)
{
    (void)arg;
    for (int i = 0; i < N_ITERS; i++) {
        /* Not atomic: load, add one, store. Updates get lost. */
        counter++;
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