/* p3a_stack_size.c - choose a thread's stack size.
   Build: gcc -Wall -pthread -o p3a_stack_size p3a_stack_size.c */
#include <pthread.h>
#include <stdio.h>

static void *worker(void *arg)
{
    (void)arg;
    printf("worker running with the requested stack\n");
    return NULL;
}

int main(void)
{
    pthread_attr_t attr;
    pthread_t tid;
    size_t size;

    /* An attribute object holds settings for new threads. */
    pthread_attr_init(&attr);

    /* Ask for a 256 KiB stack (the default is usually 8 MiB). */
    pthread_attr_setstacksize(&attr, 256 * 1024);

    /* Read the value back to confirm it. */
    pthread_attr_getstacksize(&attr, &size);
    printf("stack size set to %zu KiB\n", size / 1024);

    /* Pass &attr as the 2nd argument instead of NULL. */
    pthread_create(&tid, &attr, worker, NULL);
    pthread_join(tid, NULL);

    pthread_attr_destroy(&attr);
    return 0;
}