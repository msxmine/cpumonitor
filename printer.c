#include "analyzer.h"
#include "ringbuffer.h"
#include "printer.h"
#include "logger.h"
#include <stdio.h>


static struct ringbuffer* recvpipe_printr;

static void system_results_free(void* sr){
    free(((struct system_results*)(sr))->data);
    free(sr);
}

static void printResults(struct system_results* sr){
    for(unsigned int i = 0; i < sr->num_cores; i++){
        printf("CPU %d : %f\n", sr->data[i].core_id, sr->data[i].usage_percent);
    }
}

void initPrinter(struct ringbuffer* in){
    recvpipe_printr = in;
}

void destroyPrinter(void){
    destroyRingBuffer(recvpipe_printr, system_results_free);
}

void processPrinter(void){
    dlog("processing printer\n");
    struct system_results* recres = (struct system_results*)(ringBufferReadTimed(recvpipe_printr, 1));
    if (recres != NULL){
        printResults(recres);
        system_results_free(recres);
    }
}
