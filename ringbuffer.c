#include <stdlib.h>
#include <pthread.h>
#include "ringbuffer.h"

void ringBufferWrite(struct ringbuffer* rb, void* dpointer){
    pthread_mutex_lock(&(rb->rb_lock));
    while(((rb->write_idx+1)%(rb->dlen)) == (rb->read_idx)){
        pthread_cond_wait(&(rb->data_change), &(rb->rb_lock));
    }
    rb->data[rb->write_idx] = dpointer;
    rb->write_idx = (rb->write_idx+1)%(rb->dlen);
    pthread_cond_signal(&(rb->data_change));
    pthread_mutex_unlock(&(rb->rb_lock));
}

void* ringBufferRead(struct ringbuffer* rb){
    pthread_mutex_lock(&(rb->rb_lock));
    while(rb->write_idx == rb->read_idx){
        pthread_cond_wait(&(rb->data_change), &(rb->rb_lock));
    }
    void* ret = rb->data[rb->read_idx];
    rb->read_idx = (rb->read_idx+1)%(rb->dlen);

    pthread_cond_signal(&(rb->data_change));
    pthread_mutex_unlock(&(rb->rb_lock));
    return ret;
}

struct ringbuffer newRingBuffer(){
    struct ringbuffer ret;
    ret.data = malloc(sizeof(void*)*200);
    ret.dlen = 200;
    ret.read_idx = 0;
    ret.write_idx = 0;
    pthread_cond_init(&(ret.data_change), NULL);
    pthread_mutex_init(&(ret.rb_lock), NULL);
    return ret;
}

void destroyRingBuffer(struct ringbuffer* rb){
    free(rb->data);
    pthread_cond_destroy(&(rb->data_change));
    pthread_mutex_destroy(&(rb->rb_lock));
}