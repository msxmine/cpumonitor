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

    sigset_t waitonsigs;
    sigemptyset(&waitonsigs);
    sigaddset(&waitonsigs, SIGTERM);
    sigaddset(&waitonsigs, SIGINT);

    pthread_sigmask(SIG_BLOCK, &waitonsigs, NULL);
    create_thread(processReader, 1, "Reader");
    create_thread(processAnalyzer,0, "Analyzer");
    create_thread(processPrinter,0, "Printer");
    create_thread(processLogger,0, "Logger");
    create_thread(watchdog, 1, "Watchdog");
    pthread_sigmask(SIG_UNBLOCK, &waitonsigs, NULL);

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
    destroyThreadManager();

}