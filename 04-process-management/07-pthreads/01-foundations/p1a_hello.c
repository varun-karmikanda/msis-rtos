/* p1a_hello.c - the smallest threaded program.
   Build: gcc -Wall -pthread -o p1a_hello p1a_hello.c */
#include <pthread.h>
#include <stdio.h>

/* A thread function takes one void * and returns a void *. */
static void *hello(void *arg)
{
    (void)arg;
    printf("Hello from the new thread\n");
    return NULL;
}

int main(void)
{
    pthread_t tid;

    /* Start hello() in a new thread: default attributes, no arg. */
    pthread_create(&tid, NULL, hello, NULL);

    /* Wait for it to finish before main returns. */
    pthread_join(tid, NULL);

    printf("Thread finished, main exits\n");
    return 0;
}