#include <stdio.h>
#include "cpu/cpu.h"
#include "loadFile/readBinaryFile.h"
#include "memory/memory.h"

int main(void) {
    int returnInit = initFile();

    if(returnInit == 1) {
        return 1;
    }

    CPU->STACK_PTR = 0xFF;
    CPU->ALL_FLAGS = 0x36; // the default values of the status register.

    // PROCESSOR INITIALIZATION ^

    puts("PROCESSOR STATUS AT START:");
    printcpuinfo();

    while(!STOP) {
        FDC();
        printcpuinfo();
    }
    return 0;
}
