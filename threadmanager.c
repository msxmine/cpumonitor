#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "threadmanager.h"
#include "timeutils.h"
#include <time.h>

struct thread_info {
    pthread_t thr;
    void (*inner_function)(void);
    struct timespec last_wd_kick;
    int loopdelay;
    pthread_mutex_t thrlock;
};



struct thread_info** registered_threads = NULL;
pthread_cond_t loop_thread_signal;
int num_registered = 0;
int thread_manager_exit = 0;

void* threadloop(void* ti_v){
    struct thread_info* ti = (struct thread_info*)(ti_v);
    pthread_mutex_lock(&(ti->thrlock));
    while(!thread_manager_exit){
        (ti->inner_function)();
        struct timespec ts = get_time_in_future(ti->loopdelay);
        pthread_cond_timedwait(&(loop_thread_signal), &(ti->thrlock), &ts);
        clock_gettime(CLOCK_MONOTONIC, &(ti->last_wd_kick));
    }
    printf("thread end\n");
    pthread_mutex_unlock(&(ti->thrlock));
}

void create_thread(void (*inner_function)(void), int loopdelay){
    if (num_registered == 0){
    pthread_cond_init(&loop_thread_signal, NULL);
    }
    registered_threads = realloc(registered_threads, sizeof(struct thread_info*)*(num_registered+1));
    registered_threads[num_registered] = malloc(sizeof(struct thread_info));
    registered_threads[num_registered]->inner_function = inner_function;
    registered_threads[num_registered]->loopdelay = loopdelay;
    pthread_mutex_init(&(registered_threads[num_registered]->thrlock), NULL);
    pthread_create(&(registered_threads[num_registered]->thr), NULL, threadloop, (void*)(registered_threads[num_registered]));
    clock_gettime(CLOCK_MONOTONIC, &(registered_threads[num_registered]->last_wd_kick));
    num_registered++;



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

void exitThreads(){
    thread_manager_exit = 1;
}

void joinThreads(){
    pthread_cond_broadcast(&loop_thread_signal);
    for (int i = 0; i < num_registered; i++){
        pthread_join(registered_threads[i]->thr, NULL);
    }
}

/*
int main(){
    create_thread(hello);
    start_wd();
    sleep(99);
}*/