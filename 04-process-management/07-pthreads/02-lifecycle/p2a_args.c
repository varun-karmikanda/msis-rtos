/* p2a_args.c - give each thread its own input.
   Build: gcc -Wall -pthread -o p2a_args p2a_args.c */
#include <pthread.h>
#include <stdio.h>

#define N 3

/* The 4th argument of pthread_create arrives here as arg. */
static void *worker(void *arg)
{
    /* Cast the void * back to the type we passed in. */
    int id = *(int *)arg;

    printf("worker got id %d\n", id);
    return NULL;
}

int main(void)
{
    pthread_t tid[N];

    /* One slot per thread, so no two threads share an argument. */
    int ids[N];

    for (int i = 0; i < N; i++) {
        ids[i] = i;
        /* 4th argument: the address of this thread's own slot. */
        pthread_create(&tid[i], NULL, worker, &ids[i]);
    }

    for (int i = 0; i < N; i++)
        pthread_join(tid[i], NULL);

    return 0;
}