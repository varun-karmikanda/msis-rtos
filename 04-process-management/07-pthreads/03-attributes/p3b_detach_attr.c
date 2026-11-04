/* p3b_detach_attr.c - create a thread that is already detached.
   Build: make p3b_detach_attr */
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static void *worker(void *arg)
{
    (void)arg;
    printf("worker: detached from the start\n");
    return NULL;
}

int main(void)
{
    pthread_attr_t attr;
    pthread_t tid;

    pthread_attr_init(&attr);

    /* Like pthread_detach, but with no gap after pthread_create. */
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&tid, &attr, worker, NULL);
    pthread_attr_destroy(&attr);

    /* Detached, so we cannot join. Sleep for the demo only. */
    sleep(1);
    return 0;
}