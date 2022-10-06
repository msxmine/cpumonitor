#include "ringbuffer.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "logger.h"

struct ringbuffer logbuffer;

void initLogger(){
    logbuffer = newRingBuffer();
}

void destroyLogger(){
    destroyRingBuffer(&logbuffer);
}

void processLogger(){
    char* msg = (char*)ringBufferRead(&logbuffer);
    printf("%s", msg);
    free(msg);
}

void dlog(char* msg){
    size_t msglen = strlen(msg);
    char* msgcopy = malloc(sizeof(char)*(msglen+1));
    memcpy(msgcopy, msg, (msglen+1));
    ringBufferWrite(&logbuffer, msgcopy);
}