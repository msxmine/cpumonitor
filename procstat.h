#pragma once

#include <stdint.h>

struct core_stats {
    uint64_t user;
    uint64_t nice;
    uint64_t system;
    uint64_t idle;
    uint64_t iowait;
    uint64_t irq;
    uint64_t softirq;
    uint64_t steal;
    uint64_t guest;
    uint64_t guest_nice;
    int core_id;
};

struct system_stats {
    unsigned int num_cores;
    struct core_stats* cores;
};
