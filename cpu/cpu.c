#include <stdint.h>
#include <stdio.h>
#include "cpu.h"
#include "../memory/memory.h"


//while PC is 16 bits, using it to access memory from PROGRAM_MEM like PC + 1
//evaluates to a number that's greater than the 16 bit limit. We can use
//(uin8_t) (PC + 1) instead.

struct CPU_t *CPU = &(struct CPU_t) {0};

int STOP = 0; // if program should stop

// PRINTING CPU INFORMATION

int LastInstruction = 0;

void printcpuinfo(void) {
    printf("0x%X PC| 0x%X SP| 0x%X A| 0x%X X| 0x%X Y| 0x%X SR| 0x%X LI\n",
            CPU->PROGRAM_COUNTER, CPU->STACK_PTR, CPU->ACCUMULATOR, CPU->IRX, CPU->IRY, CPU->ALL_FLAGS, LastInstruction);
}

// ADDRESSING MODES FUNCTIONS
// these are common ways for instructions to get values corresponding with the instruction
// like for loading an immediate value into a register. Instead of just copying everything several 
// times over, we use the function, because they're all more or less the same, and this allows for
// it to be more readable and flexible.

int imm(void) {
    return (uint8_t) (CPU->PROGRAM_COUNTER + 1);
}

int zpg(void) {
    return MEMORY->PROGRAM_MEM[(uint8_t) (CPU->PROGRAM_COUNTER + 1)];
}

int zpgX(void) {
    return MEMORY->PROGRAM_MEM[(uint8_t) ((CPU->PROGRAM_COUNTER + 1) + CPU->IRX)];
}

int zpgY(void) {
    return MEMORY->PROGRAM_MEM[(uint8_t) ((CPU->PROGRAM_COUNTER + 1) + CPU->IRY)];
}

int abso(void) {
    return READ16(MEMORY->PROGRAM_MEM[(uint8_t) (CPU->PROGRAM_COUNTER + 1)]);
}

int absx(void) {
    return (uint8_t) (READ16(MEMORY->PROGRAM_MEM[(uint8_t) CPU->PROGRAM_COUNTER + 1]) + CPU->IRX);
}

int absy(void) {
    return (uint8_t) (READ16(MEMORY->PROGRAM_MEM[(uint8_t) (CPU->PROGRAM_COUNTER + 1)]) + CPU->IRY);
}

int Xind(void) {
    uint8_t x = CPU->IRX + MEMORY->PROGRAM_MEM[(uint8_t) (CPU->PROGRAM_COUNTER + 1)];
    return (READ16(MEMORY->PROGRAM_MEM[x]));
}
int indY(void) {
    uint8_t TA = MEMORY->PROGRAM_MEM[(uint8_t) (CPU->PROGRAM_COUNTER + 1)];
    return (uint8_t) READ16((MEMORY->PROGRAM_MEM[TA]) + CPU->IRY);
}

// FLAG CHECK FUNCTIONS 

void N_FLAGCHECK(int check) {
    CPU->FLAGS.NEGATIVE_FLAG = (check >> 7);
}

void Z_FLAGCHECK(int check) {
    if(check == 0x0) {
        CPU->FLAGS.ZERO_FLAG = 1;
        return;
    }
    CPU->FLAGS.ZERO_FLAG = 0;
}

void V_FLAGCHECK(int check) {
    if(check > 127 || check < -128) {
        CPU->FLAGS.OVERFLOW_FLAG = 1;
        return;
    }
    CPU->FLAGS.OVERFLOW_FLAG = 0;
}

void C_FLAGCHECK(int check) {
    CPU->FLAGS.CARRY_FLAG = (check >> 8);
}

    // FETCH DECODE EXECUTE

void FDC(void) {
    LastInstruction = MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER];
    switch(MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER]) {
        
        // LDA

        case 0xA9:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[imm()];
            CPU->PROGRAM_COUNTER += 2;
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            break;
        case 0xA5:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[zpg()];
            CPU->PROGRAM_COUNTER += 2;
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            break;
        case 0xB5:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[zpgX()];
            CPU->PROGRAM_COUNTER += 2;
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            break;
        case 0xAD:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[abso()];
            CPU->PROGRAM_COUNTER += 3;
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            break;
        case 0xBD:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[absx()];
            CPU->PROGRAM_COUNTER += 3;
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            break;
        case 0xB9:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[absy()];
            CPU->PROGRAM_COUNTER += 3;
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            break;
        case 0xA1:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[Xind()];
            CPU->PROGRAM_COUNTER += 2;
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            break;
        case 0xB1:
            CPU->ACCUMULATOR = MEMORY->PROGRAM_MEM[indY()];
            CPU->PROGRAM_COUNTER += 2;
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            break;

        // LDX

        case 0xA2:
            CPU->IRX = MEMORY->PROGRAM_MEM[imm()];
            CPU->PROGRAM_COUNTER += 2;
            N_FLAGCHECK(CPU->IRX);
            Z_FLAGCHECK(CPU->IRX);
            break;
        case 0xA6:
            CPU->IRX = MEMORY->PROGRAM_MEM[zpg()];
            CPU->PROGRAM_COUNTER += 2;
            N_FLAGCHECK(CPU->IRX);
            Z_FLAGCHECK(CPU->IRX);
            break;
        case 0xB6:
            CPU->IRX = MEMORY->PROGRAM_MEM[zpgY()];
            CPU->PROGRAM_COUNTER += 2;
            N_FLAGCHECK(CPU->IRX);
            Z_FLAGCHECK(CPU->IRX);
            break;
        case 0xAE:
            CPU->IRX = MEMORY->PROGRAM_MEM[abso()];
            CPU->PROGRAM_COUNTER += 3;
            N_FLAGCHECK(CPU->IRX);
            Z_FLAGCHECK(CPU->IRX);
            break;
        case 0xBE:
            CPU->IRX = MEMORY->PROGRAM_MEM[absy()];
            CPU->PROGRAM_COUNTER += 3;
            N_FLAGCHECK(CPU->IRX);
            Z_FLAGCHECK(CPU->IRX);
            break;
    
        // LDY

        case 0xA0:
            CPU->IRY = MEMORY->PROGRAM_MEM[imm()];
            CPU->PROGRAM_COUNTER += 2;
            N_FLAGCHECK(CPU->IRY);
            Z_FLAGCHECK(CPU->IRY);
            break;
        case 0xA4:
            CPU->IRY = MEMORY->PROGRAM_MEM[zpg()];
            CPU->PROGRAM_COUNTER += 2;
            N_FLAGCHECK(CPU->IRY);
            Z_FLAGCHECK(CPU->IRY);
            break;
        case 0xB4:
            CPU->IRY = MEMORY->PROGRAM_MEM[zpgX()];
            CPU->PROGRAM_COUNTER += 2;
            N_FLAGCHECK(CPU->IRY);
            Z_FLAGCHECK(CPU->IRY);
            break;
        case 0xAC:
            CPU->IRY = MEMORY->PROGRAM_MEM[abso()];
            CPU->PROGRAM_COUNTER += 3;
            N_FLAGCHECK(CPU->IRY);
            Z_FLAGCHECK(CPU->IRY);
            break;
        case 0xBC:
            CPU->IRY = MEMORY->PROGRAM_MEM[absx()];
            CPU->PROGRAM_COUNTER += 3;
            N_FLAGCHECK(CPU->IRY);
            Z_FLAGCHECK(CPU->IRY);
            break;

        // STA
        
        case 0x85:
            MEMORY->PROGRAM_MEM[zpg()] = CPU->ACCUMULATOR;
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x95:
            MEMORY->PROGRAM_MEM[zpgX()] = CPU->ACCUMULATOR;
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x8D:
            MEMORY->PROGRAM_MEM[abso()] = CPU->ACCUMULATOR;
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0x9D:
            MEMORY->PROGRAM_MEM[absx()] = CPU->ACCUMULATOR;
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0x99:
            MEMORY->PROGRAM_MEM[absy()] = CPU->ACCUMULATOR;
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0x81:
            MEMORY->PROGRAM_MEM[Xind()] = CPU->ACCUMULATOR;
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x91:
            MEMORY->PROGRAM_MEM[indY()] = CPU->ACCUMULATOR;
            CPU->PROGRAM_COUNTER += 2;
            break;
        
        // STX

        case 0x86:
            MEMORY->PROGRAM_MEM[zpg()] = CPU->IRX;
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x96:
            MEMORY->PROGRAM_MEM[zpgX()] = CPU->IRX;
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x8E:
            MEMORY->PROGRAM_MEM[abso()] = CPU->IRX;
            CPU->PROGRAM_COUNTER += 3;
            break;

        // STY
        
        case 0x84:
            MEMORY->PROGRAM_MEM[zpg()] = CPU->IRY;
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x94:
            MEMORY->PROGRAM_MEM[zpgX()] = CPU->IRY;
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x8C:
            MEMORY->PROGRAM_MEM[abso()] = CPU->IRY;
            CPU->PROGRAM_COUNTER += 3;
            break;

        // TAX

        case 0xAA:
            CPU->IRX = CPU->ACCUMULATOR;
            CPU->PROGRAM_COUNTER += 1;
            N_FLAGCHECK(CPU->IRX);
            Z_FLAGCHECK(CPU->IRX);
            break;

        // TAY
        
        case 0xA8:
            CPU->IRY = CPU->ACCUMULATOR;
            CPU->PROGRAM_COUNTER += 1;
            N_FLAGCHECK(CPU->IRX);
            Z_FLAGCHECK(CPU->IRX);

            break;

        // TSX
        
        case 0xBA:
            CPU->IRX = CPU->STACK_PTR;
            CPU->PROGRAM_COUNTER += 1;
            N_FLAGCHECK(CPU->IRX);
            Z_FLAGCHECK(CPU->IRX);
            break;

        // TXA
        
        case 0x8A:
            CPU->ACCUMULATOR = CPU->IRX;
            CPU->PROGRAM_COUNTER += 1;
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            break;

        // TXS

        case 0x9A:
            CPU->STACK_PTR = CPU->IRX;
            CPU->PROGRAM_COUNTER += 1;
            break;

        // TYA

        case 0x98:
            CPU->ACCUMULATOR = CPU->IRY;
            CPU->PROGRAM_COUNTER += 1;
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            break;

        // STACK OPERATIONS (PHA,PHP,PLA, PLP)

        case 0x48:
            MEMORY->STACK[CPU->STACK_PTR] = CPU->ACCUMULATOR;
            CPU->STACK_PTR--;
            CPU->PROGRAM_COUNTER += 1;

            break;
        case 0x08:
            CPU->FLAGS.IGNORED = 1;
            MEMORY->STACK[CPU->STACK_PTR] = CPU->ALL_FLAGS;
            CPU->STACK_PTR--;
            CPU->PROGRAM_COUNTER += 1;

            break;
        case 0x68:
            CPU->ACCUMULATOR = MEMORY->STACK[CPU->STACK_PTR];
            CPU->STACK_PTR++;
            CPU->PROGRAM_COUNTER += 1;

            break;
        case 0x28:
            MEMORY->STACK[CPU->STACK_PTR] = CPU->ALL_FLAGS;
            CPU->STACK_PTR++;
            CPU->PROGRAM_COUNTER += 1;

            break;

        // DEC

        case 0xC6:
            MEMORY->PROGRAM_MEM[zpg()]--;
            N_FLAGCHECK(MEMORY->PROGRAM_MEM[zpg()]);
            Z_FLAGCHECK(MEMORY->PROGRAM_MEM[zpg()]);
            CPU->PROGRAM_COUNTER += 1;
            break;
        case 0xD6:
            MEMORY->PROGRAM_MEM[zpgX()]--;
            N_FLAGCHECK(MEMORY->PROGRAM_MEM[zpgX()]);
            Z_FLAGCHECK(MEMORY->PROGRAM_MEM[zpgX()]);
            CPU->PROGRAM_COUNTER += 1;
            break;
        case 0xCE:
            MEMORY->PROGRAM_MEM[abso()]--;
            N_FLAGCHECK(MEMORY->PROGRAM_MEM[abso()]);
            Z_FLAGCHECK(MEMORY->PROGRAM_MEM[abso()]);
            CPU->PROGRAM_COUNTER += 1;
            break;
        case 0xDE:
            MEMORY->PROGRAM_MEM[absx()]--;
            N_FLAGCHECK(MEMORY->PROGRAM_MEM[absx()]);
            Z_FLAGCHECK(MEMORY->PROGRAM_MEM[absx()]);
            CPU->PROGRAM_COUNTER += 1;
            break;

        // DEX
        
        case 0xCA:
            CPU->IRX--;
            N_FLAGCHECK(CPU->IRX);
            Z_FLAGCHECK(CPU->IRX);
            CPU->PROGRAM_COUNTER += 1;
            break;

        // DEY

        case 0x88:
            CPU->IRY--;
            N_FLAGCHECK(CPU->IRX);
            Z_FLAGCHECK(CPU->IRX);
            CPU->PROGRAM_COUNTER += 1;
            break;

        // INC
        
        case 0xE6:
            MEMORY->PROGRAM_MEM[zpg()]++;
            N_FLAGCHECK(MEMORY->PROGRAM_MEM[zpg()]);
            Z_FLAGCHECK(MEMORY->PROGRAM_MEM[zpg()]);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0xF6:
            MEMORY->PROGRAM_MEM[zpgX()]++;
            N_FLAGCHECK(MEMORY->PROGRAM_MEM[zpgX()]);
            Z_FLAGCHECK(MEMORY->PROGRAM_MEM[zpgX()]);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0xEE:
            MEMORY->PROGRAM_MEM[abso()]++;
            N_FLAGCHECK(MEMORY->PROGRAM_MEM[abso()]);
            Z_FLAGCHECK(MEMORY->PROGRAM_MEM[abso()]);
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0xFE:
            MEMORY->PROGRAM_MEM[absx()]++;
            N_FLAGCHECK(MEMORY->PROGRAM_MEM[absx()]);
            Z_FLAGCHECK(MEMORY->PROGRAM_MEM[absx()]);
            CPU->PROGRAM_COUNTER += 3;
            break;

        // INX

        case 0xE8:
            CPU->IRX++;
            CPU->PROGRAM_COUNTER += 1;
            break;
        
        // INY

        case 0xC8:
            CPU->IRY++;
            CPU->PROGRAM_COUNTER += 1;
            break;

       // ADC

        case 0x69:
            ADD(MEMORY->PROGRAM_MEM[imm()]);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x65:
            ADD(MEMORY->PROGRAM_MEM[zpg()]);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x75:
            CPU->ACCUMULATOR += MEMORY->PROGRAM_MEM[zpgX()];
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x6D:
            CPU->ACCUMULATOR += MEMORY->PROGRAM_MEM[abso()];
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0x7D:
            CPU->ACCUMULATOR += MEMORY->PROGRAM_MEM[absx()];
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0x79:
            CPU->ACCUMULATOR += MEMORY->PROGRAM_MEM[absy()];
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0x61:
            CPU->ACCUMULATOR += MEMORY->PROGRAM_MEM[Xind()];
            CPU->PROGRAM_COUNTER += 2; 
            break;
        case 0x71:
            CPU->ACCUMULATOR += MEMORY->PROGRAM_MEM[indY()];
            CPU->PROGRAM_COUNTER += 2;
            break;

        // SBC
        
        case 0xE9:
            SUB(MEMORY->PROGRAM_MEM[imm()]);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0xE5:
            SUB(MEMORY->PROGRAM_MEM[zpg()]);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0xF5:
            SUB(MEMORY->PROGRAM_MEM[zpg()]);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0xED:
            SUB(MEMORY->PROGRAM_MEM[abso()]);
            CPU->PROGRAM_COUNTER += 3; 
            break;
        case 0xFD:
            SUB(MEMORY->PROGRAM_MEM[absx()]);
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0xF9:
            SUB(MEMORY->PROGRAM_MEM[absy()]);
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0xE1:
            SUB(MEMORY->PROGRAM_MEM[Xind()]);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0xF1:
            SUB(MEMORY->PROGRAM_MEM[indY()]);
            CPU->PROGRAM_COUNTER += 2;
            break;

        // AND

        case 0x29:
            CPU->ACCUMULATOR &= MEMORY->PROGRAM_MEM[imm()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x25:
            CPU->ACCUMULATOR &= MEMORY->PROGRAM_MEM[zpg()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x35:
            CPU->ACCUMULATOR &= MEMORY->PROGRAM_MEM[zpgX()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x2D:
            CPU->ACCUMULATOR &= MEMORY->PROGRAM_MEM[abso()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0x3D:
            CPU->ACCUMULATOR &= MEMORY->PROGRAM_MEM[absx()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0x39:
            CPU->ACCUMULATOR &= MEMORY->PROGRAM_MEM[absy()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0x21:
            CPU->ACCUMULATOR &= MEMORY->PROGRAM_MEM[Xind()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x31:
            CPU->ACCUMULATOR &= MEMORY->PROGRAM_MEM[indY()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;

        // EOR

        case 0x49:
            CPU->ACCUMULATOR ^= MEMORY->PROGRAM_MEM[imm()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            
            break;
        case 0x45:
            CPU->ACCUMULATOR ^= MEMORY->PROGRAM_MEM[zpg()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x55:
            CPU->ACCUMULATOR ^= MEMORY->PROGRAM_MEM[zpgX()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x4D:
            CPU->ACCUMULATOR ^= MEMORY->PROGRAM_MEM[abso()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0x5D:
            CPU->ACCUMULATOR ^= MEMORY->PROGRAM_MEM[absx()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0x59:
            CPU->ACCUMULATOR ^= MEMORY->PROGRAM_MEM[absy()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0x41:
            CPU->ACCUMULATOR ^= MEMORY->PROGRAM_MEM[Xind()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x51:
            CPU->ACCUMULATOR ^= MEMORY->PROGRAM_MEM[indY()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;

        // ORA    

        case 0x09:
            CPU->ACCUMULATOR |= MEMORY->PROGRAM_MEM[imm()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x05:
            CPU->ACCUMULATOR |= MEMORY->PROGRAM_MEM[zpg()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x15:
            CPU->ACCUMULATOR |= MEMORY->PROGRAM_MEM[zpgX()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x0D:
            CPU->ACCUMULATOR |= MEMORY->PROGRAM_MEM[abso()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0x1D:
            CPU->ACCUMULATOR |= MEMORY->PROGRAM_MEM[absx()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0x19:
            CPU->ACCUMULATOR |= MEMORY->PROGRAM_MEM[absy()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 3;
            break;
        case 0x01:
            CPU->ACCUMULATOR |= MEMORY->PROGRAM_MEM[Xind()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;
        case 0x11:
            CPU->ACCUMULATOR |= MEMORY->PROGRAM_MEM[indY()];
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;

       // ASL

       case 0x0A:
            C_FLAGCHECK(CPU->ACCUMULATOR << 1);
            CPU->ACCUMULATOR <<= 1;
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 1;
            break;
       case 0x06:
            C_FLAGCHECK(MEMORY->PROGRAM_MEM[zpg()] << 1);
            MEMORY->PROGRAM_MEM[zpg()] <<= 1;
            N_FLAGCHECK(MEMORY->PROGRAM_MEM[zpg()]);
            Z_FLAGCHECK(MEMORY->PROGRAM_MEM[zpg()]);
            CPU->PROGRAM_COUNTER += 2;
            break;
       case 0x16:
            C_FLAGCHECK(MEMORY->PROGRAM_MEM[zpgX()] << 1);
            MEMORY->PROGRAM_MEM[zpgX()] <<= 1;
            N_FLAGCHECK(MEMORY->PROGRAM_MEM[zpgX()]);
            Z_FLAGCHECK(MEMORY->PROGRAM_MEM[zpgX()]);
            CPU->PROGRAM_COUNTER += 2;
            break;
       case 0x0E:
            C_FLAGCHECK(MEMORY->PROGRAM_MEM[abso()] << 1);
            MEMORY->PROGRAM_MEM[abso()] <<= 1;
            N_FLAGCHECK(MEMORY->PROGRAM_MEM[abso()]);
            Z_FLAGCHECK(MEMORY->PROGRAM_MEM[abso()]);
            CPU->PROGRAM_COUNTER += 3;
            break;
       case 0x1E:
            C_FLAGCHECK(MEMORY->PROGRAM_MEM[absx()] << 1);
            MEMORY->PROGRAM_MEM[absx()] <<= 1;
            N_FLAGCHECK(MEMORY->PROGRAM_MEM[absx()]);
            Z_FLAGCHECK(MEMORY->PROGRAM_MEM[absx()]);
            CPU->PROGRAM_COUNTER += 3;
            break;

       // LSR

       case 0x4A:
            C_FLAGCHECK(CPU->ACCUMULATOR >> 1);
            CPU->ACCUMULATOR >>= 1;
            N_FLAGCHECK(CPU->ACCUMULATOR);
            Z_FLAGCHECK(CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 1;
            break;
       case 0x46:
            C_FLAGCHECK(MEMORY->PROGRAM_MEM[zpg()] >> 1);
            MEMORY->PROGRAM_MEM[zpg()] >>= 1;
            N_FLAGCHECK(MEMORY->PROGRAM_MEM[zpg()]);
            Z_FLAGCHECK(MEMORY->PROGRAM_MEM[zpg()]);
            CPU->PROGRAM_COUNTER += 2;
            break;
       case 0x56:
            C_FLAGCHECK(MEMORY->PROGRAM_MEM[zpgX()] >> 1);
            MEMORY->PROGRAM_MEM[zpgX()] >>= 1;
            N_FLAGCHECK(MEMORY->PROGRAM_MEM[zpgX()]);
            Z_FLAGCHECK(MEMORY->PROGRAM_MEM[zpgX()]);
            CPU->PROGRAM_COUNTER += 2;
            break;
       case 0x4E:
            C_FLAGCHECK(MEMORY->PROGRAM_MEM[abso()] >> 1);
            MEMORY->PROGRAM_MEM[abso()] >>= 1;
            N_FLAGCHECK(MEMORY->PROGRAM_MEM[abso()]);
            Z_FLAGCHECK(MEMORY->PROGRAM_MEM[abso()]);
            CPU->PROGRAM_COUNTER += 3;
            break;
       case 0x5E:
            C_FLAGCHECK(MEMORY->PROGRAM_MEM[absx()] >> 1);
            MEMORY->PROGRAM_MEM[absx()] >>= 1;
            N_FLAGCHECK(MEMORY->PROGRAM_MEM[absx()]);
            Z_FLAGCHECK(MEMORY->PROGRAM_MEM[absx()]);
            CPU->PROGRAM_COUNTER += 3;
            break;

        // ROL

       case 0x2A:
            CPU->FLAGS.CARRY_FLAG = CPU->ACCUMULATOR >> 7;
            CPU->ACCUMULATOR >>= 1;
            CPU->ACCUMULATOR |= CPU->FLAGS.CARRY_FLAG;
            break;
       case 0x26:
            CPU->FLAGS.CARRY_FLAG = MEMORY->PROGRAM_MEM[zpg()] >> 7;
            MEMORY->PROGRAM_MEM[zpg()] >>= 1;
            MEMORY->PROGRAM_MEM[zpg()] |= CPU->FLAGS.CARRY_FLAG;
            break;
       case 0x36:
            CPU->FLAGS.CARRY_FLAG = MEMORY->PROGRAM_MEM[zpgX()] >> 7;
            MEMORY->PROGRAM_MEM[zpgX()] >>= 1;
            MEMORY->PROGRAM_MEM[zpgX()] |= CPU->FLAGS.CARRY_FLAG;
            break;
       case 0x2E:
            CPU->FLAGS.CARRY_FLAG = MEMORY->PROGRAM_MEM[abso()] >> 7;
            MEMORY->PROGRAM_MEM[abso()] >>= 1;
            MEMORY->PROGRAM_MEM[abso()] |= CPU->FLAGS.CARRY_FLAG;
            break;
       case 0x3E:
            CPU->FLAGS.CARRY_FLAG = MEMORY->PROGRAM_MEM[absx()] >> 7;
            MEMORY->PROGRAM_MEM[absx()] >>= 1;
            MEMORY->PROGRAM_MEM[absx()] |= CPU->FLAGS.CARRY_FLAG;
            break;

       // ROR

       case 0x6A:
            CPU->FLAGS.CARRY_FLAG = CPU->ACCUMULATOR << 7;
            CPU->ACCUMULATOR <<= 1;
            CPU->ACCUMULATOR |= CPU->FLAGS.CARRY_FLAG >> 7;
            break;
       case 0x66:
            CPU->FLAGS.CARRY_FLAG = MEMORY->PROGRAM_MEM[zpg()] << 7;
            MEMORY->PROGRAM_MEM[zpg()] <<= 1;
            MEMORY->PROGRAM_MEM[zpg()] |= CPU->FLAGS.CARRY_FLAG >> 7;
            break;
       case 0x76:
            CPU->FLAGS.CARRY_FLAG = MEMORY->PROGRAM_MEM[zpg()] << 7;
            MEMORY->PROGRAM_MEM[zpgX()] <<= 1;
            MEMORY->PROGRAM_MEM[zpgX()] |= CPU->FLAGS.CARRY_FLAG >> 7;
            break;
       case 0x6E:
            CPU->FLAGS.CARRY_FLAG = MEMORY->PROGRAM_MEM[zpg()] << 7;
            MEMORY->PROGRAM_MEM[abso()] <<= 1;
            MEMORY->PROGRAM_MEM[abso()] |= CPU->FLAGS.CARRY_FLAG >> 7;
            break;
       case 0x7E:
            CPU->FLAGS.CARRY_FLAG = MEMORY->PROGRAM_MEM[zpg()] << 7;
            MEMORY->PROGRAM_MEM[absx()] <<= 1;
            MEMORY->PROGRAM_MEM[absx()] |= CPU->FLAGS.CARRY_FLAG >> 7;
            break;


       // CLEAR FLAGS (CLC, CLD, CLI , CLV)

       case 0x18:
            CPU->FLAGS.CARRY_FLAG = 0;
            break;
       case 0xD8:
            CPU->FLAGS.DECIMAL_FLAG = 0;
            break;
       case 0x58:
            CPU->FLAGS.INTERRUPT_FLAG = 0;
            break;
       case 0xB8:
            CPU->FLAGS.OVERFLOW_FLAG = 0;
            break;

       // SET FLAGS (SEC, SED, SEI)

       case 0x38:
            CPU->FLAGS.CARRY_FLAG = 1;
            CPU->PROGRAM_COUNTER += 1;
            break;
       case 0xF8:
            CPU->FLAGS.DECIMAL_FLAG = 1;
            break;
       case 0x78:
            CPU->FLAGS.INTERRUPT_FLAG = 1;
            break;

       // CMP
       
       case 0xC9:
            comp(MEMORY->PROGRAM_MEM[imm()], CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;
       case 0xC5:
            comp(MEMORY->PROGRAM_MEM[zpg()], CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;
       case 0xD5:
            comp(MEMORY->PROGRAM_MEM[zpgX()], CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;
       case 0xCD:
            comp(MEMORY->PROGRAM_MEM[abso()], CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 3;
            break;
       case 0xDD:
            comp(MEMORY->PROGRAM_MEM[absx()], CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 3;
            break;
       case 0xD9:
            comp(MEMORY->PROGRAM_MEM[absy()], CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 3;
            break;
       case 0xC1:
            comp(MEMORY->PROGRAM_MEM[Xind()], CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;
       case 0xD1:
            comp(MEMORY->PROGRAM_MEM[indY()], CPU->ACCUMULATOR);
            CPU->PROGRAM_COUNTER += 2;
            break;

        // CPX

        case 0xE0:
            comp(MEMORY->PROGRAM_MEM[imm()], CPU->IRX);
            break;
        case 0xE4:
            comp(MEMORY->PROGRAM_MEM[zpg()], CPU->IRX);
            break;
        case 0xEC:
            comp(MEMORY->PROGRAM_MEM[abso()], CPU->IRX);
            break;
        
        // CPY 

        case 0xC0:
            comp(MEMORY->PROGRAM_MEM[imm()], CPU->IRY);
            break;
        case 0xC4:
            comp(MEMORY->PROGRAM_MEM[zpg()], CPU->IRY);
            break;
        case 0xCC:
            comp(MEMORY->PROGRAM_MEM[abso()], CPU->IRY);
            break;

        // BCC
    
        case 0x90:
            if(!CPU->FLAGS.CARRY_FLAG) {
                CPU->PROGRAM_COUNTER += MEMORY->PROGRAM_MEM[(uint8_t) (CPU->PROGRAM_COUNTER + 1)];
                break;
            }
            CPU->PROGRAM_COUNTER += 2;
            break;

        // BCS
        
        case 0xB0:
            if(CPU->FLAGS.CARRY_FLAG) {
                CPU->PROGRAM_COUNTER += MEMORY->PROGRAM_MEM[(uint8_t) (CPU->PROGRAM_COUNTER + 1)];
                break;
            }
            CPU->PROGRAM_COUNTER += 2;
            break;

        // BEQ
        
        case 0xF0:
            if(CPU->FLAGS.ZERO_FLAG) {
                CPU->PROGRAM_COUNTER += MEMORY->PROGRAM_MEM[(uint8_t) (CPU->PROGRAM_COUNTER + 1)];
            }
            CPU->PROGRAM_COUNTER += 2;
            break;

        //  BMI

        case 0x30:
            if(CPU->FLAGS.NEGATIVE_FLAG) {
                CPU->PROGRAM_COUNTER += MEMORY->PROGRAM_MEM[(uint8_t) (CPU->PROGRAM_COUNTER + 1)];
            }
            CPU->PROGRAM_COUNTER += 2;
            break;

        // BNE

        case 0xD0:
            if(!CPU->FLAGS.ZERO_FLAG) {
                CPU->PROGRAM_COUNTER += MEMORY->PROGRAM_MEM[(uint8_t) (CPU->PROGRAM_COUNTER + 1)];
            }
            CPU->PROGRAM_COUNTER += 2;
            break;

        // BPL

        case 0x10:
            if(!CPU->FLAGS.NEGATIVE_FLAG) {
                CPU->PROGRAM_COUNTER += MEMORY->PROGRAM_MEM[(uint8_t) (CPU->PROGRAM_COUNTER + 1)];
            }
            CPU->PROGRAM_COUNTER += 2;
            break;

        // BVC

        case 0x50:
            if(!CPU->FLAGS.OVERFLOW_FLAG) {
                CPU->PROGRAM_COUNTER += MEMORY->PROGRAM_MEM[(uint8_t) (CPU->PROGRAM_COUNTER + 1)];
            }
            CPU->PROGRAM_COUNTER += 2;
            break;

        // BVS

        case 0x70:
            if(CPU->FLAGS.OVERFLOW_FLAG) {
                CPU->PROGRAM_COUNTER += MEMORY->PROGRAM_MEM[(uint8_t) (CPU->PROGRAM_COUNTER + 1)];
            }
            CPU->PROGRAM_COUNTER += 2;
            break;
        
        // JMP

        case 0x4C:
            CPU->PROGRAM_COUNTER = abso();
            break;
        case 0x6C:
            CPU->PROGRAM_COUNTER = MEMORY->PROGRAM_MEM[abso()];
            break;

        // JSR

        case 0x20:
            MEMORY->STACK[CPU->STACK_PTR] = MEMORY->PROGRAM_MEM[(uint8_t) (CPU->PROGRAM_COUNTER + 2)];
            CPU->STACK_PTR--;
            CPU->PROGRAM_COUNTER = abso();
            break;

        // RTI
        // "Break flag and bit 5 is ignored, when pulling into the status register (CPU->ALL_fLAGS)"
        // Keep that in mind
        
        case 0x40:
            CPU->ALL_FLAGS = MEMORY->STACK[CPU->STACK_PTR];
            CPU->STACK_PTR++;
            CPU->PROGRAM_COUNTER = MEMORY->STACK[CPU->STACK_PTR];
            CPU->STACK_PTR++;
            break;

        // BIT
        
        case 0x24:
            CPU->ALL_FLAGS |= ( 6 << (6 >> MEMORY->PROGRAM_MEM[zpg()]));
            CPU->FLAGS.ZERO_FLAG = (MEMORY->PROGRAM_MEM[zpg()] & CPU->ACCUMULATOR);
            break;
        case 0x2c:
            CPU->ALL_FLAGS |= ( 6 << (6 >> MEMORY->PROGRAM_MEM[abso()]));
            CPU->FLAGS.ZERO_FLAG = (MEMORY->PROGRAM_MEM[zpg()] & CPU->ACCUMULATOR);
            break;

        // NOP

        // This instruction does literally nothing :)

        case 0xEA:
            CPU->PROGRAM_COUNTER += 1;
            break;

        // BRK

        case 0x00:
             MEMORY->STACK[CPU->STACK_PTR] = MEMORY->PROGRAM_MEM[(uint8_t) (CPU->PROGRAM_COUNTER + 2)];
             CPU->STACK_PTR--;
             CPU->FLAGS.BREAK_FLAG = 1;
             MEMORY->STACK[CPU->STACK_PTR] = CPU->ALL_FLAGS;
             CPU->STACK_PTR--;
             puts("BREAK CALLED | PROCESSOR STATUS AT BREAK:");
             STOP = 1;
             CPU->PROGRAM_COUNTER += 1;
             break;
        
        default: // if the processor tries to decode and execute an instruction not defined above.
            puts("\nERROR! Instruction not valid | PROCESSOR STATUS AT BREAK:");
            STOP = 1;
            break;
    }
}


// MISC FUNCTIONS

void ADD(uint8_t x) {
    uint16_t result = (CPU->ACCUMULATOR + x) + CPU->FLAGS.CARRY_FLAG;
    C_FLAGCHECK(result);
    V_FLAGCHECK(result);
    N_FLAGCHECK(result);
    Z_FLAGCHECK(result);
    CPU->ACCUMULATOR = result & 0xFF;
}

void SUB(uint8_t x) {
    x = ~x;
    ADD(x);
}

// for comparing, with the branch instructions
void comp(uint8_t o, uint8_t r) {
    uint16_t result = r - o;
    N_FLAGCHECK(result);
    Z_FLAGCHECK(result);
    C_FLAGCHECK(result);
}

