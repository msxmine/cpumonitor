#include "doublebuf.h"
#include "procreader.h"
#include "analyzer.h"
#include "printer.h"
#include "ringbuffer.h"
#include "threadmanager.h"
#include "logger.h"
#include <unistd.h>
#include <stdio.h>

int main(){
    struct doublebuffer db = newBuffer();
    struct ringbuffer rb = newRingBuffer();
    initReader("./hello", &db);
    initAnalyzer(&db, &rb);
    initPrinter(&rb);
    initLogger();

    create_thread(processReader);
    create_thread(processAnalyzer);
    create_thread(processPrinter);
    create_thread(processLogger);

    sleep(5);

    exitThreads();
    closeBuffer(&db);
    closeRingBuffer(&rb);
    closeLogger();

    joinThreads();

}