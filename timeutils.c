#include <time.h>
#include "timeutils.h"

struct timespec get_delta_time(struct timespec* start, struct timespec* end){
    struct timespec delta;
    delta.tv_nsec = (end->tv_nsec - start->tv_nsec);
    delta.tv_sec = (end->tv_sec - start->tv_sec);
    if (end->tv_nsec < start->tv_nsec){
        delta.tv_sec -= 1;
        delta.tv_nsec += 1000000000;
    }
    return delta;
}

struct timespec get_time_in_future(int sec){
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    res.tv_sec += sec;
    return res;
}