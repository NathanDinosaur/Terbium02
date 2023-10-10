#include <stdio.h>
#include "cpu/cpu.h"
#include "loadFile/readBinaryFile.h"
#include "memory/memory.h"

int main(void) {
/*    CPU->PROGRAM_COUNTER = 0X0;
    CPU->IRY = 0x00;
    MEMORY->ZERO_PAGE[0x11] = 0x69;
    MEMORY->ZERO_PAGE[0x12] = 0x69;
    MEMORY->PROGRAM_MEM[1] = 0x11;
    MEMORY->PROGRAM_MEM[26985] = 0x70;
    MEMORY->PROGRAM_MEM[4370] = 0x69;
*/
    CPU->PROGRAM_COUNTER = 1;
    MEMORY->PROGRAM_MEM[1] = 0xB4;
    LDA(imm);
    printf("%d\n", CPU->ACCUMULATOR);
    //initFile(); // segmentation fault :weary:
    return 0;
}
