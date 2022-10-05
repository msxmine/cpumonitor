#include "analyzer.h"
#include "ringbuffer.h"
#include "printer.h"
#include <stdio.h>


struct ringbuffer* recvpipe_printr;

void printResults(struct system_results* sr){
    for(int i = 0; i < sr->num_cores; i++){
        printf("CPU %d : %f\n", sr->data[i].core_id, sr->data[i].usage_percent);
    }
}

struct system_results* recvBufPrinter(){
    return (struct system_results*)(ringBufferRead(recvpipe_printr));
}

void initPrinter(struct ringbuffer* in){
    recvpipe_printr = in;
}

void processPrinter(){
    printf("processing printer\n");
    struct system_results* recres = recvBufPrinter();
    printResults(recres);
    free(recres->data);
    free(recres);
}