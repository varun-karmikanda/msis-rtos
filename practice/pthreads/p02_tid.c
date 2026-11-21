#include<pthread.h>
#include<stdio.h>
#include<unistd.h>

int shared = 67;
static int thread_no = 1;

static void *worker(void *args){
    int private = 63;

    pid_t pid = getpid();
    pthread_t tid = pthread_self();

    printf("\nTHREAD_%d {\n\tpid_%d: %d, \n\ttid_%d: %lu,", thread_no, thread_no, pid, thread_no, tid);
    printf("\n\tvalues: {\n\t\tprivate_%d: %d, \n\t\tshared_%d: %d,\n\t},", thread_no, private, thread_no, shared);
    printf("\n\taddress: {\n\t\t&private_%d: %p, \n\t\t&shared_%d: %p,\n\t}\n}", thread_no, &private, thread_no, &shared);

    thread_no++;

    return NULL;
}

int main(void){
    pthread_t pid_1, pid_2;

    printf("\n----- pthread -----");

    pthread_create(&pid_1, NULL, worker, NULL);
    pthread_create(&pid_2, NULL, worker, NULL);

    pthread_join(pid_1, NULL);
    pthread_join(pid_2, NULL);

    return 0;
}