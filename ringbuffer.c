#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "ringbuffer.h"
#include "timeutils.h"
#include <errno.h>

//Simple synchronized ringbuffer
static int rbWaitForUnlock(struct ringbuffer* rb, int timeout){
    if (timeout == -1){
        pthread_cond_wait(&(rb->data_change), &(rb->rb_lock));
        return 0;
    } else {
        struct timespec ts = get_time_in_future(timeout);
        return pthread_cond_timedwait(&(rb->data_change), &(rb->rb_lock), &ts) == ETIMEDOUT ? 1 : 0;
    }
}

int ringBufferWrite(struct ringbuffer* rb, void* dpointer){
    return ringBufferWriteTimed(rb, dpointer, -1);
}

int ringBufferWriteTimed(struct ringbuffer* rb, void* dpointer, int timeout){
    pthread_mutex_lock(&(rb->rb_lock));
    while(((rb->write_idx+1)%(rb->dlen)) == (rb->read_idx) || rb->closed){
        if (rb->closed || rbWaitForUnlock(rb, timeout)){
            pthread_mutex_unlock(&(rb->rb_lock));
            return 1;
        }
    }
    rb->data[rb->write_idx] = dpointer;
    rb->write_idx = (rb->write_idx+1)%(rb->dlen);
    pthread_cond_signal(&(rb->data_change));
    pthread_mutex_unlock(&(rb->rb_lock));
    return 0;
}

void* ringBufferRead(struct ringbuffer* rb){
    return ringBufferReadTimed(rb, -1);
}

void* ringBufferReadTimed(struct ringbuffer* rb, int timeout){
    pthread_mutex_lock(&(rb->rb_lock));
    while(rb->write_idx == rb->read_idx || rb->closed){
        if (rb->closed || rbWaitForUnlock(rb, timeout)){
            pthread_mutex_unlock(&(rb->rb_lock));
            return NULL;
        }
    }
    void* ret = rb->data[rb->read_idx];
    rb->read_idx = (rb->read_idx+1)%(rb->dlen);

    pthread_cond_signal(&(rb->data_change));
    pthread_mutex_unlock(&(rb->rb_lock));
    return ret;
}

struct ringbuffer newRingBuffer(void){
    struct ringbuffer ret;
    ret.data = malloc(sizeof(void*)*200);
    ret.dlen = 200;
    ret.read_idx = 0;
    ret.write_idx = 0;
    ret.closed = 0;
    pthread_cond_init(&(ret.data_change), NULL);
    pthread_mutex_init(&(ret.rb_lock), NULL);
    return ret;
}

void closeRingBuffer(struct ringbuffer* rb){
    rb->closed = 1;
    pthread_cond_broadcast(&(rb->data_change));
}

void destroyRingBuffer(struct ringbuffer* rb, void (*cleanup_function)(void*)){
    while(rb->write_idx != rb->read_idx){
        cleanup_function(rb->data[rb->read_idx]);
        rb->read_idx = (rb->read_idx+1)%(rb->dlen);
    }
    free(rb->data);
    pthread_cond_destroy(&(rb->data_change));
    pthread_mutex_destroy(&(rb->rb_lock));
}
