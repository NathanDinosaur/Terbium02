#include <stdint.h>

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

extern struct CPU_t *CPU;

void TYA(void);

void TXS(void);

void PHA(void);

void PHP(void);

void PLA(void);

void PLP(void);

void LDA(ADDR_MODE AM);

#endif
