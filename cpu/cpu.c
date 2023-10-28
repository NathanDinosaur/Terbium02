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

       // ADC

        case 0x69:
            CPU->ACCUMULATOR += MEMORY->PROGRAM_MEM[imm()];
            CPU->ACCUMULATOR += CPU->FLAGS.CARRY_FLAG;
            CPU->FLAGS.CARRY_FLAG = 0;
            break;
        case 0x65:
            CPU->ACCUMULATOR += MEMORY->PROGRAM_MEM[zpg()];
            CPU->ACCUMULATOR += CPU->FLAGS.CARRY_FLAG;
            CPU->FLAGS.CARRY_FLAG = 0;
            break;
        case 0x75:
            CPU->ACCUMULATOR += MEMORY->PROGRAM_MEM[zpgX()];
            CPU->ACCUMULATOR += CPU->FLAGS.CARRY_FLAG;
            CPU->FLAGS.CARRY_FLAG = 0;
            break;
        case 0x6D:
            CPU->ACCUMULATOR += MEMORY->PROGRAM_MEM[abso()];
            CPU->ACCUMULATOR += CPU->FLAGS.CARRY_FLAG;
            CPU->FLAGS.CARRY_FLAG = 0;
            break;
        case 0x7D:
            CPU->ACCUMULATOR += MEMORY->PROGRAM_MEM[absx()];
            CPU->ACCUMULATOR += CPU->FLAGS.CARRY_FLAG;
            CPU->FLAGS.CARRY_FLAG = 0;
            break;
        case 0x79:
            CPU->ACCUMULATOR += MEMORY->PROGRAM_MEM[absy()];
            CPU->ACCUMULATOR += CPU->FLAGS.CARRY_FLAG;
            CPU->FLAGS.CARRY_FLAG = 0;
            break;
        case 0x61:
            CPU->ACCUMULATOR += MEMORY->PROGRAM_MEM[Xind()];
            CPU->ACCUMULATOR += CPU->FLAGS.CARRY_FLAG;
            CPU->FLAGS.CARRY_FLAG = 0;
            break;
        case 0x71:
            CPU->ACCUMULATOR += MEMORY->PROGRAM_MEM[indY()];
            CPU->ACCUMULATOR += CPU->FLAGS.CARRY_FLAG;
            CPU->FLAGS.CARRY_FLAG = 0;
            break;

        // SBC
        
        case 0xE9:
            CPU->ACCUMULATOR -= MEMORY->PROGRAM_MEM[imm()];
            CPU->ACCUMULATOR -= CPU->FLAGS.CARRY_FLAG;
            CPU->FLAGS.CARRY_FLAG = 0;
            break;
        case 0xE5:
            CPU->ACCUMULATOR -= MEMORY->PROGRAM_MEM[zpg()];
            CPU->ACCUMULATOR -= CPU->FLAGS.CARRY_FLAG;
            CPU->FLAGS.CARRY_FLAG = 0;
            break;
        case 0xF5:
            CPU->ACCUMULATOR -= MEMORY->PROGRAM_MEM[zpgX()];
            CPU->ACCUMULATOR -= CPU->FLAGS.CARRY_FLAG;
            CPU->FLAGS.CARRY_FLAG = 0;
            break;
        case 0xED:
            CPU->ACCUMULATOR -= MEMORY->PROGRAM_MEM[abso()];
            CPU->ACCUMULATOR -= CPU->FLAGS.CARRY_FLAG;
            CPU->FLAGS.CARRY_FLAG = 0;
            break;
        case 0xFD:
            CPU->ACCUMULATOR -= MEMORY->PROGRAM_MEM[absx()];
            CPU->ACCUMULATOR -= CPU->FLAGS.CARRY_FLAG;
            CPU->FLAGS.CARRY_FLAG = 0;
            break;
        case 0xF9:
            CPU->ACCUMULATOR -= MEMORY->PROGRAM_MEM[absy()];
            CPU->ACCUMULATOR -= CPU->FLAGS.CARRY_FLAG;
            CPU->FLAGS.CARRY_FLAG = 0;
            break;
        case 0xE1:
            CPU->ACCUMULATOR -= MEMORY->PROGRAM_MEM[Xind()];
            CPU->ACCUMULATOR -= CPU->FLAGS.CARRY_FLAG;
            CPU->FLAGS.CARRY_FLAG = 0;
            break;
        case 0xF1:
            CPU->ACCUMULATOR -= MEMORY->PROGRAM_MEM[indY()];
            CPU->ACCUMULATOR -= CPU->FLAGS.CARRY_FLAG;
            CPU->FLAGS.CARRY_FLAG = 0;
            break;

        // AND

        case 0x29:
            CPU->ACCUMULATOR &= MEMORY->PROGRAM_MEM[imm()];
            break;
        case 0x25:
            CPU->ACCUMULATOR &= MEMORY->PROGRAM_MEM[zpg()];
            break;
        case 0x35:
            CPU->ACCUMULATOR &= MEMORY->PROGRAM_MEM[zpgX()];
            break;
        case 0x2D:
            CPU->ACCUMULATOR &= MEMORY->PROGRAM_MEM[abso()];
            break;
        case 0x3D:
            CPU->ACCUMULATOR &= MEMORY->PROGRAM_MEM[absx()];
            break;
        case 0x39:
            CPU->ACCUMULATOR &= MEMORY->PROGRAM_MEM[absy()];
            break;
        case 0x21:
            CPU->ACCUMULATOR &= MEMORY->PROGRAM_MEM[Xind()];
            break;
        case 0x31:
            CPU->ACCUMULATOR &= MEMORY->PROGRAM_MEM[indY()];
            break;

        // EOR

        case 0x49:
            CPU->ACCUMULATOR ^= MEMORY->PROGRAM_MEM[imm()];
            break;
        case 0x45:
            CPU->ACCUMULATOR ^= MEMORY->PROGRAM_MEM[zpg()];
            break;
        case 0x55:
            CPU->ACCUMULATOR ^= MEMORY->PROGRAM_MEM[zpgX()];
            break;
        case 0x4D:
            CPU->ACCUMULATOR ^= MEMORY->PROGRAM_MEM[abso()];
            break;
        case 0x5D:
            CPU->ACCUMULATOR ^= MEMORY->PROGRAM_MEM[absx()];
            break;
        case 0x59:
            CPU->ACCUMULATOR ^= MEMORY->PROGRAM_MEM[absy()];
            break;
        case 0x41:
            CPU->ACCUMULATOR ^= MEMORY->PROGRAM_MEM[Xind()];
            break;
        case 0x51:
            CPU->ACCUMULATOR ^= MEMORY->PROGRAM_MEM[indY()];
            break;

        // ORA    

        case 0x09:
            CPU->ACCUMULATOR |= MEMORY->PROGRAM_MEM[imm()];
            break;
        case 0x05:
            CPU->ACCUMULATOR |= MEMORY->PROGRAM_MEM[zpg()];
            break;
        case 0x15:
            CPU->ACCUMULATOR |= MEMORY->PROGRAM_MEM[zpgX()];
            break;
        case 0x0D:
            CPU->ACCUMULATOR |= MEMORY->PROGRAM_MEM[abso()];
            break;
        case 0x1D:
            CPU->ACCUMULATOR |= MEMORY->PROGRAM_MEM[absx()];
            break;
        case 0x19:
            CPU->ACCUMULATOR |= MEMORY->PROGRAM_MEM[absy()];
            break;
        case 0x01:
            CPU->ACCUMULATOR |= MEMORY->PROGRAM_MEM[Xind()];
            break;
        case 0x11:
            CPU->ACCUMULATOR |= MEMORY->PROGRAM_MEM[indY()];
            break;

       // ASL

       case 0x0A:
            CPU->FLAGS.CARRY_FLAG = CPU->ACCUMULATOR >> 7;
            CPU->ACCUMULATOR >>= 1;
            break;
       case 0x06:
            CPU->FLAGS.CARRY_FLAG = MEMORY->PROGRAM_MEM[zpg()] >> 7;
            MEMORY->PROGRAM_MEM[zpg()] >>= 1;
            break;
       case 0x16:
            CPU->FLAGS.CARRY_FLAG = MEMORY->PROGRAM_MEM[zpgX()] >> 7;
            MEMORY->PROGRAM_MEM[zpgX()] >>= 1;
            break;
       case 0x0E:
            CPU->FLAGS.CARRY_FLAG = MEMORY->PROGRAM_MEM[abso()] >> 7;
            MEMORY->PROGRAM_MEM[abso()] >>= 1;
            break;
       case 0x1E:
            CPU->FLAGS.CARRY_FLAG = MEMORY->PROGRAM_MEM[absx()] >> 7;
            MEMORY->PROGRAM_MEM[absx()] >>= 1;
            break;

       // LSR

       case 0x4A:
            CPU->FLAGS.CARRY_FLAG = CPU->ACCUMULATOR << 7;
            CPU->ACCUMULATOR <<= 1;
            break;
       case 0x46:
            CPU->FLAGS.CARRY_FLAG = MEMORY->PROGRAM_MEM[zpg()] << 7;
            MEMORY->PROGRAM_MEM[zpg()] <<= 1;
            break;
       case 0x56:
            CPU->FLAGS.CARRY_FLAG = MEMORY->PROGRAM_MEM[zpg()] << 7;
            MEMORY->PROGRAM_MEM[zpgX()] <<= 1;
            break;
       case 0x4E:
            CPU->FLAGS.CARRY_FLAG = MEMORY->PROGRAM_MEM[zpg()] << 7;
            MEMORY->PROGRAM_MEM[abso()] <<= 1;
            break;
       case 0x5E:
            CPU->FLAGS.CARRY_FLAG = MEMORY->PROGRAM_MEM[zpg()] << 7;
            MEMORY->PROGRAM_MEM[absx()] <<= 1;
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
            break;
       case 0xF8:
            CPU->FLAGS.DECIMAL_FLAG = 1;
            break;
       case 0x78:
            CPU->FLAGS.INTERRUPT_FLAG = 1;
            break;

       // CMP
       
       case 0xc9:
            comp(MEMORY->PROGRAM_MEM[imm()], CPU->ACCUMULATOR);
            break;
       case 0xc5:
            comp(MEMORY->PROGRAM_MEM[zpg()], CPU->ACCUMULATOR);
            break;
       case 0xD5:
            comp(MEMORY->PROGRAM_MEM[zpgX()], CPU->ACCUMULATOR);
            break;
       case 0xCD:
            comp(MEMORY->PROGRAM_MEM[abso()], CPU->ACCUMULATOR);
            break;
       case 0xDD:
            comp(MEMORY->PROGRAM_MEM[absx()], CPU->ACCUMULATOR);
            break;
       case 0xD9:
            comp(MEMORY->PROGRAM_MEM[absy()], CPU->ACCUMULATOR);
            break;
       case 0xC1:
            comp(MEMORY->PROGRAM_MEM[Xind()], CPU->ACCUMULATOR);
            break;
       case 0xD1:
            comp(MEMORY->PROGRAM_MEM[indY()], CPU->ACCUMULATOR);
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
                CPU->PROGRAM_COUNTER += (int) MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1];
            }
            break;

        // BCS
        
        case 0xB0:
            if(CPU->FLAGS.CARRY_FLAG) {
                CPU->PROGRAM_COUNTER += (int) MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1];
            }
            break;

        // BEQ
        
        case 0xF0:
             if(CPU->FLAGS.ZERO_FLAG) {
                CPU->PROGRAM_COUNTER += (int) MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1];
            }
            break;

        //  BMI

        case 0x30:
             if(CPU->FLAGS.NEGATIVE_FLAG) {
                CPU->PROGRAM_COUNTER += (int) MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1];
            }
            break;

        // BNE

        case 0xD0:
            if(!CPU->FLAGS.ZERO_FLAG) {
                CPU->PROGRAM_COUNTER += (int) MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1];
            }
            break;

        // BPL

        case 0x10:
             if(!CPU->FLAGS.NEGATIVE_FLAG) {
                CPU->PROGRAM_COUNTER += (int) MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1];
            }
            break;

        // BVC

        case 0x50:
             if(!CPU->FLAGS.OVERFLOW_FLAG) {
                CPU->PROGRAM_COUNTER += (int) MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1];
            }
            break;

        // BVS

        case 0x70:
             if(CPU->FLAGS.OVERFLOW_FLAG) {
                CPU->PROGRAM_COUNTER += (int) MEMORY->PROGRAM_MEM[CPU->PROGRAM_COUNTER + 1];
            }
            break;
        
        // JMP

        case 0x4C:
            CPU->PROGRAM_COUNTER = abso();
            break;
        case 0x6C:
            //CPU->PROGRAM_COUNTER =



        default:
            puts("\nERROR! Instruction not valid");
            break;
    }
}


void comp(uint8_t r, uint8_t o) {
    uint8_t sr = r - o;
    if(r < o) {
        CPU->FLAGS.NEGATIVE_FLAG = sr << 7;
        CPU->FLAGS.ZERO_FLAG = 0;
        CPU->FLAGS.CARRY_FLAG = 0;
    }
    else if(r == 0) {
        CPU->FLAGS.ZERO_FLAG = 1;
        CPU->FLAGS.CARRY_FLAG = 1;
        CPU->FLAGS.NEGATIVE_FLAG = 0;
    }
    else if(r > o) {
        CPU->FLAGS.ZERO_FLAG = 0;
        CPU->FLAGS.CARRY_FLAG = 1;
        CPU->FLAGS.NEGATIVE_FLAG = sr << 7;
    }
}

