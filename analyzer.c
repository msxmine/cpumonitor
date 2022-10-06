#include "analyzer.h"
#include "procstat.h"
#include "doublebuf.h"
#include "ringbuffer.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

void analyze(struct system_stats* in, struct system_stats* prev, struct core_results* out, int num_cores){
    for (int cidx = 0; cidx < num_cores; cidx++){
        struct core_stats* pc = &(prev->cores[cidx]);
        struct core_stats* cc = &(in->cores[cidx]);
        uint64_t previdle = (pc->idle + pc->iowait);
        uint64_t prevactive = (pc->user + pc->nice + pc->system + pc->irq + pc->softirq + pc->steal);
        uint64_t curidle = (cc->idle + cc->iowait);
        uint64_t curactive = (cc->user + cc->nice + cc->system + cc->irq + cc->softirq + cc->steal);
        uint64_t prevtotal = previdle + prevactive;
        uint64_t curtotal = curidle + curactive;
        uint64_t inttotal = curtotal - prevtotal;
        uint64_t intidle = curidle - previdle;
        double core_prec = (double)(inttotal-intidle)/(double)(inttotal);
        out[cidx].core_id = cc->core_id;
        out[cidx].usage_percent = core_prec;
    }
}

struct system_stats previous_stats;
struct system_stats current_stats;

struct doublebuffer* recvpipe;
struct ringbuffer* outpipe;
int received = 0;

int recvBuf(){
    size_t bufsize;
    void* recvdata;
    if (readNewTimed(recvpipe, &recvdata, &bufsize,1)){
        return 1;
    }

    if (previous_stats.cores != NULL){
        free(previous_stats.cores);
    }
    previous_stats = current_stats;

    current_stats.num_cores = bufsize / sizeof(struct core_stats);
    printf("received cores %d\n", current_stats.num_cores);
    current_stats.cores = (struct core_stats*)recvdata;
    received++;
    return 0;
}

void sendData(struct system_results* sr){
    if (ringBufferWrite(outpipe, sr)){
        free(sr->data);
        free(sr);
    }
}

void initAnalyzer(struct doublebuffer* recv, struct ringbuffer* out){
    recvpipe = recv;
    outpipe = out;
}

void destroyAnalyzer(){
    destroyBuffer(recvpipe);
}

void processAnalyzer(){
    printf("processing anaalyzer\n");
    if (recvBuf()){
        return;
    }
    printf("receiving\n");
    if (received > 1){
        int coresnum = current_stats.num_cores;
        printf("cores %d %d\n", coresnum, previous_stats.num_cores);
        struct core_results* results = malloc(sizeof(struct core_results)*coresnum);
        analyze(&current_stats, &previous_stats, results, coresnum);
        struct system_results* sr = malloc(sizeof(struct system_results));
        sr->num_cores = coresnum;
        sr->data = results;
        sendData(sr);
    }
}