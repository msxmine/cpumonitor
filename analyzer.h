#include "doublebuf.h"
#include "ringbuffer.h"
#include "procstat.h"

struct core_results {
    int core_id;
    double usage_percent;
};

struct system_results {
    unsigned int num_cores;
    struct core_results* data;
};


void initAnalyzer(struct doublebuffer* recv, struct ringbuffer* out);
void processAnalyzer(void);
void destroyAnalyzer(void);
void analyze(struct system_stats* in, struct system_stats* prev, struct core_results* out, unsigned int num_cores);
