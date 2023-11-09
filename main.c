#include <stdio.h>
#include "cpu/cpu.h"
#include "loadFile/readBinaryFile.h"
#include "memory/memory.h"

int main(void) {
    initFile(); 
    CPU->STACK_PTR = 255;
    CPU->ACCUMULATOR = 0;

    MEMORY->PROGRAM_MEM[0] = 0xE8;

    while(!STOP) {
        FDC();
        printcpuinfo();
    }
    return 0;
}
