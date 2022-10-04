#include <pthread.h>
#include <string.h>

struct doublebuffer {
    void* bufa;
    void* bufb;
    int lastwrite;
    int newdata;
    pthread_cond_t newdata_signal;
    pthread_mutex_t bufa_lock;
    pthread_mutex_t bufb_lock;
    pthread_mutex_t db_lock;

};

void writeBuf(struct doublebuffer* db, void* databuf, size_t datalen){
    pthread_mutex_lock(&(db->db_lock));
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
        memcpy(db->bufa, databuf, datalen);
        pthread_mutex_unlock(&(db->bufa_lock));
    } else {
        memcpy(db->bufb, databuf, datalen);
        pthread_mutex_unlock(&(db->bufb_lock));
    }
    db->lastwrite = lockedbuf;
    db->newdata = 1;
    pthread_cond_broadcast(&(db->newdata_signal));
    pthread_mutex_unlock(&(db->db_lock));
}

void readNew(struct doublebuffer* db, void* resbuf, size_t datalen){
    pthread_mutex_lock(&(db->db_lock));
    while (!(db->newdata)){
        pthread_cond_wait(&(db->newdata_signal), &(db->db_lock));
    }
    if (db->lastwrite){
        memcpy(resbuf, db->bufb, datalen);
    } else {
        memcpy(resbuf, db->bufa, datalen);
    }
    pthread_mutex_unlock(&(db->db_lock));
}