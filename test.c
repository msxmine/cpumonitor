#include "assert.h"
#include "timeutils.h"
#include <time.h>
#include "procstat.h"
#include <string.h>
#include "analyzer.h"
#include <stdio.h>
#include "doublebuf.h"
#include "ringbuffer.h"
#include "procreader.h"

static void timeutilsTest(void){
    struct timespec a = get_time_in_future(0);
    struct timespec b = get_time_in_future(6);
    struct timespec delta = get_delta_time(&a, &b);
    assert(delta.tv_sec >= 6);
    b.tv_sec = a.tv_sec + 6;
    b.tv_nsec = a.tv_nsec;
    delta = get_delta_time(&a, &b);
    assert(delta.tv_sec == 6);
    assert(delta.tv_nsec == 0);
}

static void analyzerTest(void){
    struct core_stats cores[2];
    struct core_stats oldcores[2];
    struct system_stats stat;
    struct system_stats oldstat;
    stat.num_cores = 2;
    stat.cores = cores;
    oldstat.num_cores = 2;
    oldstat.cores = oldcores;
    memset(cores,0, sizeof(struct core_stats)*2);
    memset(oldcores,0, sizeof(struct core_stats)*2);
    cores[0].idle = 2;
    cores[1].idle = 4;
    cores[0].user = 2;
    cores[1].user = 0;
    oldcores[0].idle = 1;
    oldcores[1].idle = 2;
    oldcores[0].user = 1;
    oldcores[1].user = 0;
    struct core_results res[2];
    analyze(&stat, &oldstat, res, 2);
    assert(res[0].usage_percent == 0.5);
    assert(res[1].usage_percent == 0.0);
}

static void doublebufTest(void){
    struct doublebuffer db = newBuffer();
    void* resbuf;
    char teststr[] = "testString";
    size_t rlen;
    assert(readNewTimed(&db, &resbuf, &rlen, 0) == 1);
    writeBuf(&db, teststr, sizeof(teststr));
    assert(readNewTimed(&db, &resbuf, &rlen, 0) == 0);
    assert(rlen == sizeof(teststr));
    assert(memcmp(resbuf, teststr, sizeof(teststr)) == 0);
    free(resbuf);
    closeBuffer(&db);
    destroyBuffer(&db);
}

static void ringbufTest(void){
    struct ringbuffer rb = newRingBuffer();
    char teststr[] = "RingBuffer";
    char teststr2[] = "Testing";
    assert(ringBufferReadTimed(&rb, 0) == NULL);
    ringBufferWrite(&rb, teststr);
    ringBufferWrite(&rb, teststr2);
    assert(ringBufferRead(&rb) == teststr);
    assert(ringBufferRead(&rb) == teststr2);
    assert(ringBufferReadTimed(&rb, 0) == NULL);
    closeRingBuffer(&rb);
    destroyRingBuffer(&rb, free);
}

static void procreaderTest(void){
    FILE* testfile = fopen("./testdata/teststats.txt", "r");
    struct system_stats res;
    res.cores = NULL;
    readProc(testfile, &res);
    assert(res.num_cores == 24);
    assert(res.cores[0].core_id == 0);
    assert(res.cores[0].user == 1112431);
    assert(res.cores[23].core_id == 23);
    assert(res.cores[23].iowait == 27649);
    free(res.cores);
    fclose(testfile);
}

int main(void){
    timeutilsTest();
    printf("Timeutils tested\n");
    analyzerTest();
    printf("Analyzer tested\n");
    doublebufTest();
    printf("DoubleBuffer tested\n");
    ringbufTest();
    printf("RingBuffer tested\n");
    procreaderTest();
    printf("ProcReader tested\n");
    return 0;
}
