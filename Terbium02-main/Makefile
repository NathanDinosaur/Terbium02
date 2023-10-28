CC     = gcc
CFLAGS  = -std=c11 -Wall -Werror -Wpedantic -Wextra -flto -O2 -pipe
LIBS =
OBJECTS = main.o cpu/cpu.o memory/memory.o loadFile/readBinaryFile.o
BIN     = main

$(BIN): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f *.o $(BIN)

.PHONY: clean
