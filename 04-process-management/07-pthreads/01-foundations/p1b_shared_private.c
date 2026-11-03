/* p1b_shared_private.c - shared versus private data.
   Build: make p1b_shared_private */
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

/* One copy exists, so every thread sees the same variable. */
int shared = 100;

static void *worker(void *arg)
{
    /* A local variable lives on this thread's own stack. */
    int mine = 1;

    /* getpid(): same process ID in every thread. */
    pid_t pid = getpid();

    /* pthread_self(): a different thread ID in each thread. */
    pthread_t me = pthread_self();

    (void)arg;
    printf("pid=%d thread=%lu\n", pid, (unsigned long)me);
    printf("  &shared=%p &mine=%p\n",
           (void *)&shared, (void *)&mine);

    /* Stay alive so both threads exist at the same time. */
    sleep(1);
    return NULL;
}

int main(void)
{
    pthread_t t1, t2;

    pthread_create(&t1, NULL, worker, NULL);
    pthread_create(&t2, NULL, worker, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}