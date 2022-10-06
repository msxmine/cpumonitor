#pragma once

#include <time.h>

struct timespec get_delta_time(struct timespec* start, struct timespec* end);
struct timespec get_time_in_future(int sec);
