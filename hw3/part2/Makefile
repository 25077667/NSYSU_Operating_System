CC = g++
CFLAGS = -Wall -W -Og -g
CPPFLAGS = -Wall -W -Og -g -std=c++17
OUT = part2.elf

all: dep
	$(CC) -o $(OUT) *.o -lpthread

dep: 
	$(CC) $(CPPFLAGS) -c monitor.cpp
	$(CC) $(CPPFLAGS) -c philosopher.cpp
	$(CC) $(CPPFLAGS) -c main.cpp

clean:
	rm -f $(OUT) *.o .gdb_history peda-session-*

check:
	valgrind -v --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(OUT)

help:
	@printf "make clean dep all:			to make clean and all\n"
	@printf "make check				to make check memory by valgrind\n"
