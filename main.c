#include <stdio.h>
#include "cpu/cpu.h"
#include "loadFile/readBinaryFile.h"
#include "memory/memory.h"

int main(void) {
    int returnInit = initFile();
    if(returnInit == 1) {
        return 1;
    }

    CPU->STACK_PTR = 255;
    CPU->ACCUMULATOR = 0;

    // PROCESSOR INITIALIZATION ^

    while(!STOP) {
        FDC();
        printcpuinfo();
    }
    return 0;
}
