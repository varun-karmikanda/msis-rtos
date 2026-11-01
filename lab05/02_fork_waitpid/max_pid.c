#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>

int main(void){
    FILE *fp;
    long max_pid = 0;

    fp = fopen("/proc/sys/kernel/pid_max", "r");

    if(fp == NULL){
        printf("ERROR: Unable to access the system config");
        return 1;
    }

    if(fscanf(fp, "%ld", &max_pid) != 1){
        fprintf(stderr, "Error failed to parse the PID maximum constraint\n");
        fclose(fp);
        return 1;
    }

    fclose(fp);

    printf("The max pid limit = %ld", max_pid);
    
    return 0;
}