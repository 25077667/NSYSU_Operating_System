CC = g++
CFLAGS = -Wall -W -Og -g
CPPFLAGS = -Wall -W -Og -g -std=c++14
OUT = part2.elf

dep: 
	$(CC) $(CPPFLAGS) -c philosopher.cpp
	$(CC) $(CPPFLAGS) -c semaphore.cpp

all: dep
	$(CC) -o $(OUT) *.o

clean:
	rm -f $(OUT) *.o .gdb_history peda-session-*

check:
	valgrind -v --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(OUT)