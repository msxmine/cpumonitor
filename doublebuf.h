#pragma once

#include <pthread.h>
#include <string.h>
#include <stdlib.h>

struct doublebuffer {
    void* bufa;
    void* bufb;
    size_t bufa_len;
    size_t bufb_len;
    int lastwrite;
    int newdata;
    pthread_cond_t newdata_signal;
    pthread_mutex_t bufa_lock;
    pthread_mutex_t bufb_lock;
    pthread_mutex_t db_lock_rd;
    pthread_mutex_t db_lock_wr;
    int closed;
};

int writeBuf(struct doublebuffer* db, void* databuf, size_t datalen);
int readNew(struct doublebuffer* db, void** resbuf, size_t* datalen);
int readNewTimed(struct doublebuffer* db, void** resbuf, size_t* datalen, int timeout);
struct doublebuffer newBuffer(void);
void destroyBuffer(struct doublebuffer* db);
void closeBuffer(struct doublebuffer* db);
