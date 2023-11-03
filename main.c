#include <stdio.h>
#include "cpu/cpu.h"
#include "loadFile/readBinaryFile.h"
#include "memory/memory.h"

int main(void) {
    //initFile(); // segmentation fault 
    CPU->STACK_PTR = 255;

    CPU->ACCUMULATOR = 10;
    CPU->FLAGS.CARRY_FLAG = 1;
    MEMORY->PROGRAM_MEM[0] = 0x0E;
    MEMORY->PROGRAM_MEM[1] = 5;

    printf("%s\n", "Branch test");

    while(!STOP) {
        FDC();
        printcpuinfo();
    }
    return 0;
}
