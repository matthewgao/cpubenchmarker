#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

double compute_pi(size_t dt)
{
    double pi = 0.0;
    double delta = 1.0 / dt;
    for(size_t i = 0; i<dt; i++){
        double x = (double)i / dt;
        pi += delta / (1.0 + x*x);
    }
    return pi * 4.0;
}

void* run(void* arg){
    struct timespec start, finish;
    double elapsed;
    int loop = *((int*)arg);

    clock_gettime(CLOCK_MONOTONIC, &start);

    double pi;
    for(int i = 0; i< loop; i++){
        pi = compute_pi(1280000000);
    }
    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;


    printf("Thread %lx: %f Time : %f, Run %d times\n", (long)pthread_self(), pi, elapsed, loop);

    return NULL;
}

void display_usage(){
    printf("Help, A tool to make the CPU run on the full horse power.\n");
    printf("\t-t specify a thread num\n");
    printf("\t-l specify loop times per thread num\n");
    printf("\t-d thread run in detach mode\n");
}

int main(int argc, char**argv)
{
    clock_t start_time, end_time;
    int thread_num = 1;
    int process_num = 1;
    int loop_per_thread = 1;
    bool is_detach = false;
    static const char *optString = "dt:p:l:?";
    int opt = -1;
    opt = getopt( argc, argv, optString );
    
    while( opt != -1 ) {
        switch( opt ) {
            case 't':
                thread_num = atoi(optarg);
                break;
            case 'p':
                process_num = atoi(optarg);
                break;
            case 'l':
		        loop_per_thread = atoi(optarg);
		        break;
            case 'd':
                is_detach = true;
                break;
            case '?':
            default:
                display_usage();
                return 0;
        }
        opt = getopt( argc, argv, optString );
    }

    pthread_t* tid = (pthread_t*)malloc(sizeof(pthread_t)*thread_num);

    struct timespec start, finish;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &start);

    for(int i=0; i < thread_num; i++){
        pthread_create(&(tid[i]), NULL, run, &loop_per_thread);
        if(is_detach){
            pthread_detach(tid[i]);
        }
    }

    for(int i = 0; i < thread_num; i++){
        pthread_join(tid[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    printf("Time : %f\n", elapsed);
    return 0;
}
