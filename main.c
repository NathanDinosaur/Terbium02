#include <stdio.h>
#include "cpu/cpu.h"
#include "loadFile/readBinaryFile.h"
#include "memory/memory.h"

int main(void) {
    //initFile(); // segmentation fault 
    CPU->STACK_PTR = 255;
    CPU->ACCUMULATOR = 0;

    MEMORY->PROGRAM_MEM[0] = 0xA9;
    MEMORY->PROGRAM_MEM[1] = 5;

    // Hello!

    while(!STOP) {
        FDC();
        printcpuinfo();
    }
    return 0;
}
