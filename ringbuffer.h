#pragma once

#include <stdlib.h>
#include <pthread.h>

struct ringbuffer {
    void** data;
    size_t dlen;
    size_t read_idx;
    size_t write_idx;
    pthread_cond_t data_change;
    pthread_mutex_t rb_lock;
    int closed;
};

int ringBufferWrite(struct ringbuffer* rb, void* dpointer);
int ringBufferWriteTimed(struct ringbuffer* rb, void* dpointer, int timeout);
void* ringBufferRead(struct ringbuffer* rb);
void* ringBufferReadTimed(struct ringbuffer* rb, int timeout);
struct ringbuffer newRingBuffer();
void destroyRingBuffer(struct ringbuffer* rb, void (*cleanup_function)(void*));
void closeRingBuffer(struct ringbuffer* rb);


