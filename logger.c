#include "ringbuffer.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "logger.h"

struct ringbuffer logbuffer;

void initLogger(){
    logbuffer = newRingBuffer();
}

void closeLogger(){
    closeRingBuffer(&logbuffer);
}

void destroyLogger(){
    destroyRingBuffer(&logbuffer, free);
}

void processLogger(){
    char* msg = (char*)ringBufferReadTimed(&logbuffer, 1);
    if (msg == NULL){
        return;
    }
    printf("%s", msg);
    free(msg);
}

void dlog(char* msg){
    size_t msglen = strlen(msg);
    char* msgcopy = malloc(sizeof(char)*(msglen+1));
    memcpy(msgcopy, msg, (msglen+1));
    if (ringBufferWrite(&logbuffer, msgcopy)){
        free(msgcopy);
    }
}