#include "doublebuf.h"
#include "procreader.h"
#include "analyzer.h"
#include "printer.h"
#include "ringbuffer.h"
#include <unistd.h>
#include <stdio.h>

int main(){
    struct doublebuffer db = newBuffer();
    struct ringbuffer rb = newRingBuffer();
    initReader("./hello", &db);
    initAnalyzer(&db, &rb);
    initPrinter(&rb);

    for(int i = 0; i < 99; i++){
        processReader();
        processAnalyzer();
        if (i > 3){
        processPrinter();
        }
        sleep(1);
    }
}