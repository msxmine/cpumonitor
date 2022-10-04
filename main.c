#include "doublebuf.h"
#include "procreader.h"
#include "analyzer.h"
#include "unistd.h"
#include <stdio.h>

int main(){
    struct doublebuffer db = newBuffer();
    initReader("./hello", &db);
    initAnalyzer(&db);
    while(1){
        processReader();
        processAnalyzer();
        sleep(1);
    }
}