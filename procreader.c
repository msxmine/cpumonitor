#include "procstat.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int readProc(char path[], struct system_stats* result){
    FILE* fil = fopen(path, "r");

    if (fil == NULL){
        return 1;
    }
    result->num_cores = 0;

    char* linebuf = NULL;
    size_t linelen = 0;
    while(getline(&linebuf, &linelen, fil) != -1){
        printf("%s", linebuf);
        if (strlen(linebuf) > 4 && memcmp(linebuf, "cpu", 3) == 0 && linebuf[3] != ' '){
            result->cores = realloc(result->cores, sizeof(struct core_stats)*(result->num_cores+1));
            struct core_stats* target = &(result->cores[result->num_cores]);
            sscanf(linebuf+3, "%d %llu %llu %llu %llu %llu %llu %llu %llu %llu %llu", &(target->core_id), &(target->user), &(target->nice), &(target->system), &(target->idle),
                                                                                    &(target->iowait), &(target->irq), &(target->softirq), &(target->steal), &(target->guest), &(target->guest_nice));
            result->num_cores++;

        }


    }
    free(linebuf);
    return 0;
}

int main(){
    struct system_stats cpudata;
    cpudata.cores = NULL;
    cpudata.num_cores = 0;
    readProc("/proc/stat", &cpudata);
    for (int i = 0; i < cpudata.num_cores; i++){
        printf("%d %d %d\n", cpudata.cores[i].core_id, cpudata.cores[i].user, cpudata.cores[i].system);
    }
}