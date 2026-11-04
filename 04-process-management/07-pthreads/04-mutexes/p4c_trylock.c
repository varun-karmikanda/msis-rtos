/* p4c_trylock.c - try for a lock without waiting.
   Build: gcc -Wall -pthread -o p4c_trylock p4c_trylock.c */
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

static void *holder(void *arg)
{
    (void)arg;
    pthread_mutex_lock(&lock);

    /* Keep the lock for two seconds. */
    sleep(2);

    pthread_mutex_unlock(&lock);
    return NULL;
}

int main(void)
{
    pthread_t tid;

    pthread_create(&tid, NULL, holder, NULL);

    /* Give the holder time to take the lock first. */
    sleep(1);

    /* Returns 0 if we got the lock, EBUSY if someone holds it. */
    if (pthread_mutex_trylock(&lock) == EBUSY) {
        printf("busy: lock is held, doing other work\n");
    } else {
        printf("got the lock\n");
        pthread_mutex_unlock(&lock);
    }

    pthread_join(tid, NULL);
    return 0;
}