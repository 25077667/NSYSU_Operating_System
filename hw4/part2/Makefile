CC = gcc
CFLAGS = -Wall -W -Og -g -std=gnu99
OUT = part2.elf

dep: 
	$(CC) $(CFLAGS) -c mm.c
	$(CC) $(CFLAGS) -c test.c
	$(CC) $(CFLAGS) -c main.c

all: dep
	$(CC) $(CFLAGS) *.o -o $(OUT)

clean:
	rm -f $(OUT) *.o .gdb_history peda-session-* *.so

check:
	valgrind -v --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(OUT)