#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int shared = 100;

static void *worker(void *arg)
{
    int mine = 1;
    pid_t pid = getpid();
    pthread_t me = pthread_self();

    (void)arg;

    printf("pid=%d thread=%lu\n", (int)pid, (unsigned long)me);
    printf("&shared=%p &mine=%p\n",
           (void *)&shared, (void *)&mine);

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