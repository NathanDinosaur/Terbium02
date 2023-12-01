#include <stdint.h>
#ifndef MEM_H
#define MEM_H

// put memory structures and functions here :)

struct MEM_t {
    uint8_t ZERO_PAGE[256];
    uint8_t STACK[256];
    uint8_t PROGRAM_MEM[65536];
};

extern struct MEM_t *MEMORY;

#endif // MEH_H



