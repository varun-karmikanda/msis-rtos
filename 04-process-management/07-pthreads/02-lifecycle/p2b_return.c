/* p2b_return.c - get a result back from a thread.
   Build: gcc -Wall -pthread -o p2b_return p2b_return.c */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static void *square(void *arg)
{
    int n = *(int *)arg;

    /* The result must outlive the thread, so put it on the heap. */
    int *result = malloc(sizeof *result);

    *result = n * n;

    /* Returning a pointer hands it to whoever joins us. */
    return result;
}

int main(void)
{
    pthread_t tid;
    int input = 7;
    void *ret;

    pthread_create(&tid, NULL, square, &input);

    /* 2nd arg of pthread_join receives the returned pointer. */
    pthread_join(tid, &ret);

    printf("7 squared = %d\n", *(int *)ret);
    free(ret);
    return 0;
}