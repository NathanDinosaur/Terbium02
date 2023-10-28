#include <stdio.h>
#include "cpu/cpu.h"
#include "loadFile/readBinaryFile.h"
#include "memory/memory.h"

int main(void) {
    //initFile(); // segmentation fault 
    
    CPU->ACCUMULATOR = 10;
    CPU->FLAGS.CARRY_FLAG = 1;
    MEMORY->PROGRAM_MEM[0] = 0xE9;
    MEMORY->PROGRAM_MEM[1] = 5;


    FDC();
    printcpuinfo();
    printf("%d\n", CPU->ACCUMULATOR); 
    return 0;
}
