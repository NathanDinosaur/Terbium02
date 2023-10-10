#include <stdint.h>
#ifndef MEM_H
#define MEM_H


// put memory structures and functions here :)

typedef enum {
    a    =  1, // accumulator
    abso =  2, // absolute
    absX =  3, // absolute x-indexed
    absY =  4, // absolute y-indexed
    imm  =  5, // immediate 
    impl =  6, // implied
    ind  =  7, // indirect
    Xind =  8, // x-indexed indirect
    indY =  9, // indirect y-indexed
    rel  = 10, // relative
    zpg  = 11, // zeropage
    zpgX = 12, // zero page x-indexed
    zpgY = 13, // zero page y-indexed
} ADDR_MODE;

struct MEM_t {
    uint8_t ZERO_PAGE[256];
    uint8_t STACK[256];
    uint8_t PROGRAM_MEM[65536];
};

extern struct MEM_t *MEMORY;

#endif // MEH_H



