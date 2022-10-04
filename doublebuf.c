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
};

void writeBuf(struct doublebuffer* db, void* databuf, size_t datalen){
    pthread_mutex_lock(&(db->db_lock_wr));
    int lockedbuf;
    if (db->lastwrite){
        if (pthread_mutex_trylock(&(db->bufa_lock)) == 0){
            lockedbuf = 0;
        } else {
            pthread_mutex_lock(&(db->bufb_lock));
            lockedbuf = 1;
        }
    } else {
        if (pthread_mutex_trylock(&(db->bufb_lock)) == 0){
            lockedbuf = 1;
        } else {
            thread_mutex_lock(&(db->bufa_lock));
            lockedbuf = 0;
        }
    }

    if(lockedbuf){
        db->bufa = realloc(db->bufa, datalen);
        memcpy(db->bufa, databuf, datalen);
        db->bufa_len = datalen;
        pthread_mutex_unlock(&(db->bufa_lock));
    } else {
        db->bufb = realloc(db->bufb, datalen);
        memcpy(db->bufb, databuf, datalen);
        db->bufb_len = datalen;
        pthread_mutex_unlock(&(db->bufb_lock));
    }
    db->lastwrite = lockedbuf;
    db->newdata = 1;
    pthread_cond_signal(&(db->newdata_signal));
    pthread_mutex_unlock(&(db->db_lock_wr));
}

void readNew(struct doublebuffer* db, void** resbuf, size_t* datalen){
    pthread_mutex_lock(&(db->db_lock_rd));
    while (!(db->newdata)){
        pthread_cond_wait(&(db->newdata_signal), &(db->db_lock_rd));
    }
    if (db->lastwrite){
        *resbuf = malloc(db->bufb_len);
        memcpy(*resbuf, db->bufb, db->bufb_len);
        *datalen = db->bufb_len;
    } else {
        *resbuf = malloc(db->bufa_len);
        memcpy(*resbuf, db->bufa, db->bufa_len);
        *datalen = db->bufa_len;
    }
    db->newdata = 0;
    pthread_mutex_unlock(&(db->db_lock_rd));
}

struct doublebuffer newBuffer(){
    struct doublebuffer ret;
    ret.bufa = NULL;
    ret.bufb = NULL;
    ret.bufa_len = 0;
    ret.bufb_len = 0;
    ret.lastwrite = 0;
    ret.newdata = 0;
    pthread_cond_init(&(ret.bufa_lock), NULL);
    pthread_cond_init(&(ret.bufb_lock), NULL);
    pthread_cond_init(&(ret.db_lock_rd), NULL);
    pthread_cond_init(&(ret.db_lock_wr), NULL);
    return ret;
}

void destroyBuffer(struct doublebuffer* db){
    free(db->bufa);
    free(db->bufb);
}