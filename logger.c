#include "ringbuffer.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include "logger.h"

static struct ringbuffer logbuffer;
static FILE* outfile;
static pthread_mutex_t flock;

static void printout(char msg[]){
    pthread_mutex_lock(&flock);
    if (outfile != NULL){
        fprintf(outfile, "%s", msg);
    }
    pthread_mutex_unlock(&flock);
}

static void printAndFree(void* str){
    printout((char*)str);
    free(str);
}

void initLogger(char path[]){
    logbuffer = newRingBuffer();
    outfile = fopen(path, "w");
    if (outfile == NULL){
        outfile = fopen("/dev/stderr", "w");
    }
}

void closeLogger(void){
    closeRingBuffer(&logbuffer);
}

void destroyLogger(void){
    destroyRingBuffer(&logbuffer, printAndFree);
    pthread_mutex_lock(&flock);
    fclose(outfile);
    outfile = NULL;
    pthread_mutex_unlock(&flock);
}

void processLogger(void){
    char* msg = (char*)ringBufferReadTimed(&logbuffer, 1);
    if (msg == NULL){
        return;
    }
    printout(msg);
    free(msg);
}
//Main logging function
void dlog(const char* format, ...){
    va_list args;
    va_start(args, format);

    char* msgcopy = malloc(sizeof(char)*1024);
    vsnprintf(msgcopy, 1024, format, args);

    if (ringBufferWrite(&logbuffer, msgcopy)){
        //Closing fallback
        printout(msgcopy);
        free(msgcopy);
    }
}
