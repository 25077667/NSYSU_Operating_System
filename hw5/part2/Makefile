CC = gcc
CFLAGS = -Wall -W -Og -g -std=gnu99
OUT = part2.elf

all: dep
	$(CC) $(CFLAGS) *.o -o $(OUT)

dep: 
	$(CC) $(CFLAGS) -c read.c
	$(CC) $(CFLAGS) -c write.c
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f $(OUT) *.o .gdb_history peda-session-* *.so foo

check:
	valgrind -v --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(OUT)

help:
	@printf "make clean dep all:			to make clean and all\n"
	@printf "make check				to make check memory by valgrind\n"
