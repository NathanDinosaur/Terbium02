CC     = gcc
CFLAGS  = -std=c11 -Wall -Werror -Wpedantic -Wextra -flto -Ofast -pipe
LIBS =
OBJECTS = main.o cpu/cpu.o memory/memory.o loadFile/readBinaryFile.o
BIN     = main

$(BIN): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o $(BIN)

.PHONY: clean


# REPLACE -0fast WITH -02 , FAST IS NOT SAFE
