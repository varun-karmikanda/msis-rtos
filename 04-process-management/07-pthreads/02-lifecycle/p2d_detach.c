/* p2d_detach.c - a thread nobody will join.
   Build: gcc -Wall -pthread -o p2d_detach p2d_detach.c */
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

static void *background(void *arg)
{
    (void)arg;
    sleep(1);
    printf("background: done, resources freed automatically\n");
    return NULL;
}

int main(void)
{
    pthread_t tid;

    pthread_create(&tid, NULL, background, NULL);

    /* After this, joining tid is not allowed. The system frees
       the thread's resources by itself when it ends. */
    pthread_detach(tid);

    printf("main: not waiting for it\n");

    /* Demo only: keep the process alive long enough to see it. */
    sleep(2);
    return 0;
}