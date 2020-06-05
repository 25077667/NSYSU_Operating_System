CC = gcc
CFLAGS = -Wall -W -Og -g -std=gnu99
OUT = part2.elf

dep: 
	$(CC) $(CFLAGS) -c read.c
	$(CC) $(CFLAGS) -c write.c
	$(CC) $(CFLAGS) -c main.c

all: dep
	$(CC) $(CFLAGS) *.o -o $(OUT)

clean:
	rm -f $(OUT) *.o .gdb_history peda-session-* *.so foo

check:
	valgrind -v --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(OUT)