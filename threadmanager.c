#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "threadmanager.h"

struct thread_info {
    pthread_t thr;
    void (*inner_function)(void);
    struct timespec last_wd_kick;
};



struct thread_info** registered_threads = NULL;
int num_registered = 0;

void* threadloop(void* ti_v){
    struct thread_info* ti = (struct thread_info*)(ti_v);
    while(1){
    (ti->inner_function)();
    sleep(1);
    clock_gettime(CLOCK_MONOTONIC, &(ti->last_wd_kick));
    }
}

void create_thread(void (*inner_function)(void)){
    registered_threads = realloc(registered_threads, sizeof(struct thread_info*)*(num_registered+1));
    registered_threads[num_registered] = malloc(sizeof(struct thread_info));
    registered_threads[num_registered]->inner_function = inner_function;
    pthread_create(&(registered_threads[num_registered]->thr), NULL, threadloop, (void*)(registered_threads[num_registered]));
    clock_gettime(CLOCK_MONOTONIC, &(registered_threads[num_registered]->last_wd_kick));
    num_registered++;



}

struct timespec get_delta_time(struct timespec* start, struct timespec* end){
    struct timespec delta;
    delta.tv_nsec = (end->tv_nsec - start->tv_nsec);
    delta.tv_sec = (end->tv_sec - start->tv_sec);
    if (end->tv_nsec < start->tv_nsec){
        delta.tv_sec -= 1;
        delta.tv_nsec += 1000000000;
    }
    return delta;
}

void* watchdog(void* arg){
    while (1){
    for (int i = 0; i < num_registered; i++){
        struct timespec now;
        clock_gettime(CLOCK_MONOTONIC, &now);
        struct timespec delta = get_delta_time(&(registered_threads[i]->last_wd_kick), &now);
        if (delta.tv_sec > 2){
            printf("thread failed\n");
        }
    }
    sleep(1);
    }
    return NULL;
}

void start_wd(){
    pthread_t wdthr;
    pthread_create(&wdthr, NULL, watchdog, NULL);
}

void hello(){
    static int cleep = 0;
    printf("Hello\n");
    sleep(cleep);
    cleep++;
}

/*
int main(){
    create_thread(hello);
    start_wd();
    sleep(99);
}*/