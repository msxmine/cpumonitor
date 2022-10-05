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
}