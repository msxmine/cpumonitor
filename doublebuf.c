#include "doublebuf.h"
#include "timeutils.h"
#include "logger.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

//Write into an unlocked slot (with oldest data if not locked)
int writeBuf(struct doublebuffer* db, void* databuf, size_t datalen){
    if (db->closed){
        return 1;
    }
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
            pthread_mutex_lock(&(db->bufa_lock));
            lockedbuf = 0;
        }
    }

    if(!lockedbuf){
        db->bufa = realloc(db->bufa, datalen);
        memcpy(db->bufa, databuf, datalen);
        db->bufa_len = datalen;
        dlog("writing a\n");
        pthread_mutex_unlock(&(db->bufa_lock));
    } else {
        db->bufb = realloc(db->bufb, datalen);
        memcpy(db->bufb, databuf, datalen);
        db->bufb_len = datalen;
        dlog("writing b\n");
        pthread_mutex_unlock(&(db->bufb_lock));
    }
    db->lastwrite = lockedbuf;
    db->newdata = 1;
    pthread_cond_signal(&(db->newdata_signal));
    pthread_mutex_unlock(&(db->db_lock_wr));
    return 0;
}

int readNew(struct doublebuffer* db, void** resbuf, size_t* datalen){
    return readNewTimed(db, resbuf, datalen, -1);
}

static int dbWaitForUnlock(struct doublebuffer* db, int timeout){
    if (timeout == -1){
        pthread_cond_wait(&(db->newdata_signal), &(db->db_lock_rd));
        return 0;
    } else {
        struct timespec ts = get_time_in_future(timeout);
        return pthread_cond_timedwait(&(db->newdata_signal), &(db->db_lock_rd), &ts) == ETIMEDOUT ? 1 : 0;
    }
}

//Lock and read most recent complete slot
int readNewTimed(struct doublebuffer* db, void** resbuf, size_t* datalen, int timeout){
    pthread_mutex_lock(&(db->db_lock_rd));
    while (!(db->newdata) || db->closed){
        if (db->closed || dbWaitForUnlock(db, timeout)){
            pthread_mutex_unlock(&(db->db_lock_rd));
            return 1;
        }
    }
    if (db->lastwrite){
        pthread_mutex_lock(&(db->bufb_lock));
        *resbuf = malloc(db->bufb_len);
        memcpy(*resbuf, db->bufb, db->bufb_len);
        *datalen = db->bufb_len;
        dlog("reading b\n");
        pthread_mutex_unlock(&(db->bufb_lock));
    } else {
        pthread_mutex_lock(&(db->bufa_lock));
        *resbuf = malloc(db->bufa_len);
        memcpy(*resbuf, db->bufa, db->bufa_len);
        *datalen = db->bufa_len;
        dlog("reading a\n");
        pthread_mutex_unlock(&(db->bufa_lock));
    }
    db->newdata = 0;
    pthread_mutex_unlock(&(db->db_lock_rd));
    return 0;
}

struct doublebuffer newBuffer(void){
    struct doublebuffer ret;
    ret.bufa = NULL;
    ret.bufb = NULL;
    ret.bufa_len = 0;
    ret.bufb_len = 0;
    ret.lastwrite = 0;
    ret.newdata = 0;
    ret.closed = 0;
    pthread_cond_init(&(ret.newdata_signal), NULL);
    pthread_mutex_init(&(ret.bufa_lock), NULL);
    pthread_mutex_init(&(ret.bufb_lock), NULL);
    pthread_mutex_init(&(ret.db_lock_rd), NULL);
    pthread_mutex_init(&(ret.db_lock_wr), NULL);
    return ret;
}

void closeBuffer(struct doublebuffer* db){
    db->closed = 1;
    pthread_cond_broadcast(&(db->newdata_signal));
}

void destroyBuffer(struct doublebuffer* db){
    pthread_cond_destroy(&(db->newdata_signal));
    pthread_mutex_destroy(&(db->bufa_lock));
    pthread_mutex_destroy(&(db->bufb_lock));
    pthread_mutex_destroy(&(db->db_lock_rd));
    pthread_mutex_destroy(&(db->db_lock_wr));
    free(db->bufa);
    free(db->bufb);
}
