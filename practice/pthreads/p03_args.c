#include<pthread.h>
#include<stdio.h>

#define N 3

static thread_no = 1;

int arr[] = {2, 4, 6, 8, 10, 12, 14, 16, 18};

static void *worker(void *arg){
    int sum = 0;
    for (int i = 0; i < 9; i+=3)
    {
        sum += arr[i];
    }
    printf("\nTHREAD_%d sum = %d", thread_no, sum);

    thread_no++;
    
    return (void*) sum;
}

int main(void){
    int thread_count = 3;
    pthread_t tid[N];
    int start[N];

    int sum[N];
    int total = 0;

    for (int i = 0; i < thread_count; i++)
    {
        start[i] = i;
        pthread_create(&tid[i], NULL, worker, &start[i]);
    }

    for (int i = 0; i < thread_count; i++)
    {
        pthread_join(tid[i], &sum[i]);
        total += sum[i];
    }

    printf("\nTotal = %d", total);
    
}