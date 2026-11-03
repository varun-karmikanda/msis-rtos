/* p2e_loop_bug.c - the classic argument mistake.
   Build: gcc -Wall -pthread -o p2e_loop_bug p2e_loop_bug.c */
#include <pthread.h>
#include <stdio.h>

static void *worker(void *arg)
{
    /* Reads i later, after the loop may already have changed it. */
    int id = *(int *)arg;

    printf("worker sees id %d\n", id);
    return NULL;
}

int main(void)
{
    pthread_t tid[3];
    int i;

    for (i = 0; i < 3; i++) {
        /* BUG: every thread gets the address of the same i. */
        pthread_create(&tid[i], NULL, worker, &i);
    }

    for (int k = 0; k < 3; k++)
        pthread_join(tid[k], NULL);

    /* Fix: pass &ids[i] from an array, as in p2a_args.c. */
    return 0;
}