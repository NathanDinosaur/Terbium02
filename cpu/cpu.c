#include <stdint.h>
#include <stdio.h>
#include "cpu.h"
#include "../memory/memory.h"

int LastInstruction = 0;
 
#define READ16(x) *(uint16_t*) (&x)

struct CPU_t *CPU = &(struct CPU_t) {0};

// PRINTING CPU INFORMATION

void printcpuinfo(void) {
    printf("%x PC| %x SP| %x A| %x X|%x Y| 0x%x LI|\n",
            CPU->PROGRAM_COUNTER, CPU->STACK_PTR, CPU->ACCUMULATOR, CPU->IRX, CPU->IRY, LastInstruction);
}

// ADDRESSING MODES FUNCTIONS

int imm(void) {
    return CPU->PROGRAM_COUNTER + 1;
}

int zpg(void) {
    return MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1];
}

int zpgX(void) {
    return MEMORY->PROGRAM_MEM[(CPU->PROGRAM_COUNTER + 1) + CPU->IRX];
}

int zpgY(void) {
    return (CPU->PROGRAM_COUNTER + 1) + CPU->IRY;
}

int abso(void) {
    return READ16(MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1]);
}

int absx(void) {
    return READ16(MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1]) + CPU->IRX;
}

int absy(void) {
    return READ16(MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1]) + CPU->IRY;
}

int Xind(void) {
    uint8_t x = CPU->IRX + MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1];
    return (READ16(MEMORY->PROGRAM_MEM[x]));
}
int indY(void) {
    uint8_t TA = MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1];
    return READ16(MEMORY->PROGRAM_MEM[TA]) + CPU->IRY;
    // This works! Don't fuck with it
}


// FLAG FUNCTIONS

void N_FLAGCHECK(int check) {
     if(check >> 7 == 0x1) {
        CPU->FLAGS.NEGATIVE_FLAG = 1;
    }
}

void Z_FLAGCHECK(int check) {
    if(check == 0x0) {
        CPU->FLAGS.ZERO_FLAG = 1;
    }
}


// FETCH DECODE EXECUTE

void FDC(void) {
    LastInstruction = MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER];
    switch(MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER]) {
        // TRANSFER INSTRUCTIONS
        // LDA

        case 0xA9:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[imm()];
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0xA5:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[zpg()];
            break;
        case 0xB5:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[zpgX()];
            break;
        case 0xAD:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[abso()];
            break;
        case 0xBD:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[absx()];
            break;
        case 0xB9:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[absy()];
            break;
        case 0xB1:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[indY()];
            break;

        // LDX

        case 0xA2:
            CPU->IRX = MEMORY->PROGRAM_MEM[imm()];
            break;
        case 0xA6:
            CPU->IRX = MEMORY->PROGRAM_MEM[zpg()];
            break;
        case 0xB6:
            CPU->IRX = MEMORY->PROGRAM_MEM[zpgY()];
            break;
        case 0xAE:
            CPU->IRX = MEMORY->PROGRAM_MEM[abso()];
            break;
        case 0xBE:
            CPU->IRX = MEMORY->PROGRAM_MEM[absy()];
            break;
    
        // LDY

        case 0xA0:
            CPU->IRY = MEMORY->PROGRAM_MEM[imm()];
            break;
        case 0xA4:
            CPU->IRY = MEMORY->PROGRAM_MEM[zpg()];
            break;
        case 0xB4:
            CPU->IRY = MEMORY->PROGRAM_MEM[zpgX()];
            break;
        case 0xAC:
            CPU->IRY = MEMORY->PROGRAM_MEM[abso()];
            break;
        case 0xBC:
            CPU->IRY = MEMORY->PROGRAM_MEM[absx()];
            break;

        // STA
        
        case 0x85:
            MEMORY->PROGRAM_MEM[zpg()] = CPU->ACCUMULATOR;
            break;
        case 0x95:
            MEMORY->PROGRAM_MEM[zpgX()] = CPU->ACCUMULATOR;
            break;
        case 0x8D:
            MEMORY->PROGRAM_MEM[abso()] = CPU->ACCUMULATOR;
            break;
        case 0x9D:
            MEMORY->PROGRAM_MEM[absx()] = CPU->ACCUMULATOR;
            break;
        case 0x99:
            MEMORY->PROGRAM_MEM[absy()] = CPU->ACCUMULATOR;
            break;
        case 0x81:
            MEMORY->PROGRAM_MEM[Xind()] = CPU->ACCUMULATOR;
            break;
        case 0x91:
            MEMORY->PROGRAM_MEM[indY()] = CPU->ACCUMULATOR;
            break;
        
        // STX

        case 0x86:
            MEMORY->PROGRAM_MEM[zpg()] = CPU->IRX;
            break;
        case 0x96:
            MEMORY->PROGRAM_MEM[zpgX()] = CPU->IRX;
            break;
        case 0x8E:
            MEMORY->PROGRAM_MEM[abso()] = CPU->IRX;
            break;

        // STY
        
        case 0x84:
            MEMORY->PROGRAM_MEM[zpg()] = CPU->IRY;
            break;
        case 0x94:
            MEMORY->PROGRAM_MEM[zpgX()] = CPU->IRY;
            break;
        case 0x8C:
            MEMORY->PROGRAM_MEM[abso()] = CPU->IRY;
            break;

        // TAX

        case 0xAA:
            CPU->IRX = CPU->ACCUMULATOR;
            break;

        // TAY
        
        case 0xA8:
            CPU->IRY = CPU->ACCUMULATOR;
            break;

        // TSX
        
        case 0xBA:
            CPU->IRX = CPU->STACK_PTR;
            break;

        // TXA
        
        case 0x8A:
            CPU->ACCUMULATOR = CPU->IRX;
            break;

        // TXS

        case 0x9A:
            CPU->STACK_PTR = CPU->IRX;
            break;

        // TYA

        case 0x98:
            CPU->ACCUMULATOR = CPU->IRY;
            break;

        // STACK OPERATIONS (PHA,PHP,PLA, PLP)

        case 0x48:
            MEMORY->STACK[CPU->STACK_PTR] = CPU->ACCUMULATOR;
            CPU->STACK_PTR--;
            break;
        case 0x08:
            CPU->FLAGS.IGNORED = 1;
            MEMORY->STACK[CPU->STACK_PTR] = CPU->ALL_FLAGS;
            CPU->STACK_PTR--;
            break;
        case 0x68:
            CPU->ACCUMULATOR = MEMORY->STACK[CPU->STACK_PTR];
            CPU->STACK_PTR++;
            break;
        case 0x28:
            MEMORY->STACK[CPU->STACK_PTR] = CPU->ALL_FLAGS;
            CPU->STACK_PTR++;
            break;

        // DEC

        case 0xC6:
            MEMORY->PROGRAM_MEM[zpg()]--;
            break;
        case 0xD6:
            MEMORY->PROGRAM_MEM[zpgX()]--;
            break;
        case 0xCE:
            MEMORY->PROGRAM_MEM[abso()]--;
            break;
        case 0xDE:
            MEMORY->PROGRAM_MEM[absx()]--;
            break;

        // DEX
        
        case 0xCA:
            CPU->IRX--;
            break;

        // DEY

        case 0x88:
            CPU->IRY--;
            break;

        // INC
        
        case 0xE6:
            MEMORY->PROGRAM_MEM[zpg()]++;
            break;
        case 0xF6:
            MEMORY->PROGRAM_MEM[zpgX()]++;
            break;
        case 0xEE:
            MEMORY->PROGRAM_MEM[abso()]++;
            break;
        case 0xFE:
            MEMORY->PROGRAM_MEM[absx()]++;
            break;


        // INX

        case 0xE8:
            CPU->IRX++;
            break;
        
        // INY

        case 0xC8:
            CPU->IRY++;
            break;



        default:
            puts("\nERROR! Instruction not valid");
            break;
    }
}
