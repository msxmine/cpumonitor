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
};

void ringBufferWrite(struct ringbuffer* rb, void* dpointer);
void* ringBufferRead(struct ringbuffer* rb);
struct ringbuffer newRingBuffer();
void destroyRingBuffer(struct ringbuffer* rb);

