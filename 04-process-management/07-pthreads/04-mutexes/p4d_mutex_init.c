/* p4d_mutex_init.c - a mutex initialised at run time.
   Build: gcc -Wall -pthread -o p4d_mutex_init p4d_mutex_init.c */
#include <pthread.h>
#include <stdio.h>

/* The lock travels with the data it protects. */
struct account {
    pthread_mutex_t lock;
    long balance;
};

static void *deposit(void *arg)
{
    struct account *a = arg;

    for (int i = 0; i < 100000; i++) {
        pthread_mutex_lock(&a->lock);
        a->balance += 1;
        pthread_mutex_unlock(&a->lock);
    }
    return NULL;
}

int main(void)
{
    pthread_t t1, t2;
    struct account acct = { .balance = 0 };

    /* No static initializer here, so initialise it first... */
    pthread_mutex_init(&acct.lock, NULL);

    pthread_create(&t1, NULL, deposit, &acct);
    pthread_create(&t2, NULL, deposit, &acct);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("balance = %ld (expected 200000)\n", acct.balance);

    /* ...and destroy it when nobody uses it any more. */
    pthread_mutex_destroy(&acct.lock);
    return 0;
}