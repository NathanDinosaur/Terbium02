#include <stdlib.h>
#include <stdio.h>

#include "../loadFile/readBinaryFile.h"
#include "../cpu/cpu.h"

//FILE *fp = fopen("program.bin", "r");

FILE *fp;
uint8_t *PROGRAM;

void initFile(void) {
    fp = fopen("program.bin", "r");
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    PROGRAM = malloc(len);
    fread(PROGRAM, 1, len, fp);

    // we're going to need to write this to memory, at some point, but I'm not gonna add this yet because there's probably some specific place it needs to be. Writing to PROGRAM_MEMORY.
}
