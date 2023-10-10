#include <stdint.h>
#include <stdio.h>
#include "cpu.h"
#include "../memory/memory.h"

#define READ16(x) *(uint16_t*) (&x)

struct CPU_t *CPU = &(struct CPU_t) {0};

void TYA(void) {
    CPU->ACCUMULATOR = CPU->IRY;
}

void TXS(void) {
    CPU->STACK_PTR = CPU->IRX;
}

// TRANSFER INSTRUCTIONS

void LDA(ADDR_MODE AM) {
    //uint16_t assembleBytes = 0; 
    switch(AM) {
        case imm:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1];
            CPU->PROGRAM_COUNTER += 2;
            break;
        case zpg:
            CPU->ACCUMULATOR = MEMORY->ZERO_PAGE[MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1]];
            printf("%d\n", CPU->ACCUMULATOR);
            break;
            case zpgX:
            CPU->IRX += MEMORY->ZERO_PAGE[MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1]];
            CPU->ACCUMULATOR = MEMORY->ZERO_PAGE[READ16(CPU->IRX)];
            CPU->PROGRAM_COUNTER += 3;
            break;
        case abso:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[READ16(MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1])];
            CPU->PROGRAM_COUNTER += 3;
            break;
        case absX:
            CPU->IRX += 
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[READ16(MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1]) + CPU->IRX];
            CPU->PROGRAM_COUNTER += 3;
            break;
        case absY:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[READ16(MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1]) + CPU->IRY];
            CPU->PROGRAM_COUNTER += 3;
            break;
        case Xind:
            CPU->IRX += MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1];
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[READ16(MEMORY->ZERO_PAGE[CPU->IRX])];
            CPU->PROGRAM_COUNTER += 3;
            break;
        case indY:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[CPU->IRY + READ16(MEMORY->ZERO_PAGE[CPU->PROGRAM_COUNTER + 1])];
            CPU->PROGRAM_COUNTER += 3;
            break;
        default:
            puts("\nPebis");
            break;


   }  // ZERO FLAG
    if(CPU->ACCUMULATOR == 0x0) {
        CPU->FLAGS.ZERO_FLAG = 1;
    } // NEGATIVE FLAG
    if(CPU->ACCUMULATOR >> 7 == 0x1) {
        CPU->FLAGS.NEGATIVE_FLAG = 1;
    }
}


// STACK INSTRUCTIONS

void PHA(void) {
    // push Accumulator on stack
    MEMORY->STACK[CPU->STACK_PTR] = CPU->ACCUMULATOR;
    CPU->STACK_PTR--;
}

void PHP(void) {
    // push processor status on stack
    CPU->FLAGS.IGNORED = 1;
    MEMORY->STACK[CPU->STACK_PTR] = CPU->ALL_FLAGS;
    CPU->STACK_PTR--;
}

void PLA(void) {
    // pull accumuatlor from stack (into accumulator)
    CPU->ACCUMULATOR = MEMORY->STACK[CPU->STACK_PTR];
    CPU->STACK_PTR++;
}

void PLP(void) {
    // pull status register (into status register)
    MEMORY->STACK[CPU->STACK_PTR] = CPU->ALL_FLAGS;
    CPU->STACK_PTR++;
}

