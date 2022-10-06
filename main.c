#include "doublebuf.h"
#include "procreader.h"
#include "analyzer.h"
#include "printer.h"
#include "ringbuffer.h"
#include "threadmanager.h"
#include "logger.h"
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

int exit_signal_raised = 0;

void sighandler(int signum){
    exit_signal_raised = 1;
}

int main(){
    struct sigaction exithandler;
    memset(&exithandler, 0, sizeof(struct sigaction));
    exithandler.sa_handler = sighandler;
    sigaction(SIGTERM, &exithandler, NULL);
    sigaction(SIGINT, &exithandler, NULL);


    struct doublebuffer db = newBuffer();
    struct ringbuffer rb = newRingBuffer();
    initReader("./hello", &db);
    initAnalyzer(&db, &rb);
    initPrinter(&rb);
    initLogger();

    create_thread(processReader, 1);
    create_thread(processAnalyzer,0);
    create_thread(processPrinter,0);
    create_thread(processLogger,0);

    sigset_t waitonsigs;
    sigemptyset(&waitonsigs);
    sigaddset(&waitonsigs, SIGTERM);
    sigaddset(&waitonsigs, SIGINT);

    int recvsignum;
    sigwait(&waitonsigs, &recvsignum);

    exitThreads();
    closeBuffer(&db);
    closeRingBuffer(&rb);
    closeLogger();

    joinThreads();


    destroyReader();
    destroyAnalyzer();
    destroyPrinter();
    destroyLogger();


}