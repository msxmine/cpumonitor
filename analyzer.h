#include "doublebuf.h"

struct core_results {
    int core_id;
    double usage_percent;
};

struct system_results {
    int num_cores;
    struct core_results* data;
};


void initAnalyzer(struct doublebuffer* recv);
void processAnalyzer();