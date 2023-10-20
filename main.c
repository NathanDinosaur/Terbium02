#include <stdio.h>
#include "cpu/cpu.h"
#include "loadFile/readBinaryFile.h"
#include "memory/memory.h"

int main(void) {
    //initFile(); // segmentation fault 
    
    CPU->IRX = 10;
    MEMORY->PROGRAM_MEM[0] = 0xE8;
    MEMORY->PROGRAM_MEM[1] = 5;
    MEMORY->PROGRAM_MEM[5] = 15;


    FDC();
    printcpuinfo();
    printf("%d\n", CPU->ACCUMULATOR); 
    return 0;
}
