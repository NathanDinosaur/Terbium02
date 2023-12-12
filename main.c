#include <stdio.h>
#include "cpu/cpu.h"
#include "loadFile/readBinaryFile.h"
#include "memory/memory.h"

int main(void) {
    initFile();

    CPU->ALL_FLAGS = 0x36; // the default values of the status register.

    // PROCESSOR INITIALIZATION ^

    while(!STOP) {
    FDC();
    printcpuinfo();
    }
    return 0;
}
