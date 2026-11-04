/* p2c_exit.c - end a thread early with pthread_exit.
   Build: gcc -Wall -pthread -o p2c_exit p2c_exit.c */
#include <pthread.h>
#include <stdio.h>

static void helper(void)
{
    /* Ends the whole calling thread, not just this function. */
    pthread_exit((void *)42L);
}

static void *worker(void *arg)
{
    (void)arg;
    printf("worker: calling helper\n");
    helper();

    /* Never reached: pthread_exit does not return. */
    printf("worker: you will not see this\n");
    return NULL;
}

int main(void)
{
    pthread_t tid;
    void *ret;

    pthread_create(&tid, NULL, worker, NULL);
    pthread_join(tid, &ret);

    printf("main: thread exited with %ld\n", (long)ret);
    return 0;
}