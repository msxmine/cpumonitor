#include "procstat.h"
#include "procreader.h"
#include "doublebuf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <inttypes.h>
#include "logger.h"

//Read cpu data from kernel
static int readProc(FILE* fil, struct system_stats* result){
    if (fil == NULL){
        return 1;
    }
    fseek(fil, 0, SEEK_SET);
    result->num_cores = 0;

    char* linebuf = NULL;
    size_t linelen = 0;
    while(getline(&linebuf, &linelen, fil) != -1){
        if (strlen(linebuf) > 4 && memcmp(linebuf, "cpu", 3) == 0 && linebuf[3] != ' '){
            result->cores = realloc(result->cores, sizeof(struct core_stats)*(result->num_cores+1u));
            struct core_stats* target = &(result->cores[result->num_cores]);
            sscanf(linebuf+3, "%d %" SCNd64 " %" SCNd64 " %" SCNd64 " %" SCNd64 " %" SCNd64 " %" SCNd64 " %" SCNd64 " %" SCNd64 " %" SCNd64 " %" SCNd64, 
                                                                                    &(target->core_id), &(target->user), &(target->nice), &(target->system), &(target->idle),
                                                                                    &(target->iowait), &(target->irq), &(target->softirq), &(target->steal), &(target->guest), &(target->guest_nice));
            result->num_cores++;
        }
    }
    free(linebuf);
    return 0;
}

static FILE* myFile;
static struct doublebuffer* sendpipe;

void initReader(char path[], struct doublebuffer* send){
    myFile = fopen(path, "r");
    sendpipe = send;
}

void destroyReader(void){
    fclose(myFile);
}

void processReader(void){
    struct system_stats cpudata;
    cpudata.cores = NULL;
    cpudata.num_cores = 0;
    readProc(myFile, &cpudata);
    writeBuf(sendpipe, cpudata.cores, sizeof(struct core_stats)*cpudata.num_cores);
    dlog("sending kernel data\n");
    free(cpudata.cores);
    //sleep(1);
}
