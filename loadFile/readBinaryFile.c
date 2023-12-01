#include <stdlib.h>
#include <stdio.h>

#include "../loadFile/readBinaryFile.h"
#include "../cpu/cpu.h"
#include "../memory/memory.h"

FILE *fp;

int initFile(void) {
    fp = fopen("ROMs/program.bin", "r");
    if (fp == NULL) {
        puts("file invalid");
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    long len = ftell(fp);
    if(len > 65536) {
        puts("ERROR: File is too big to be read into memory.");
        return 1;
    }
    fseek(fp, 0, SEEK_SET);
    //PROGRAM = malloc(len);
    fread(MEMORY->PROGRAM_MEM, 1, len, fp);
    fclose(fp);
    return 0;
}
