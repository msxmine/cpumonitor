#include "doublebuf.h"
#include "procstat.h"
#include <stdio.h>

void initReader(char path[], struct doublebuffer* send);
void destroyReader(void);
void processReader(void);
int readProc(FILE* fil, struct system_stats* result);
