#include <stdint.h>
#include <byteswap.h>
#include "../memory/memory.h"

#ifndef CPU_H
#define CPU_H


struct CPU_t {
    uint16_t PROGRAM_COUNTER;
    uint8_t STACK_PTR; 
    uint8_t ACCUMULATOR;
    uint8_t IRX; // Index register x
    uint8_t IRY; // Index register y
    
    union {
        struct {// this is the status register
            uint8_t NEGATIVE_FLAG    : 1;
            uint8_t OVERFLOW_FLAG    : 1;
            uint8_t IGNORED          : 1;
            uint8_t BREAK_FLAG       : 1;
            uint8_t DECIMAL_FLAG     : 1;
            uint8_t INTERRUPT_FLAG   : 1;
            uint8_t ZERO_FLAG        : 1;
            uint8_t CARRY_FLAG       : 1;
        } FLAGS;
        uint8_t ALL_FLAGS;
    };
};

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define READ16(x) *(uint16_t*) (&x)

#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define READ16(x) bswap_16(*(uint16_t*) (&x))

else
#error CANNOT DETERMINE MACHINE ENDIANESS
#endif

extern int STOP;

extern struct CPU_t *CPU; // using extern with structs in C is awkward, so instead
                          // this is a pointer *to* a struct, and we extern this
                          // instead, and define the actual struct this is poin-
                          // ting to in cpu.c

void printcpuinfo(void);

// ADDRESSING MODE FUNCTION DECLERATIONS

int imm(void);
int zpg(void);
int zpgX(void);
int zpgY(void);
int abso(void);
int absx(void);
int absy(void);
int Xind(void);
int indY(void);

// FLAG CHECK FUNCTION DECLERATIONS

void N_FLAGCHECK(uint8_t check);
void Z_FLAGCHECK(uint8_t check);
void C_FLAGCHECK(uint16_t check);
void V_FLAGCHECK(int16_t check);

// FETCH, DECODE, EXECUTE

void FDC(void);

// CMP

void comp(uint8_t r, uint8_t o); 

// MISC
void ADD(uint8_t x);
void SUB(uint8_t x);

#endif
