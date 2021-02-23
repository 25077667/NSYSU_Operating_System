CC = g++
CFLAGS = -Wall -W -Og -g
CPPFLAGS = -Wall -W -Og -g -std=c++14
OUT = myshell

all: obj
	$(CC) -o $(OUT) *.o

obj: 
	gcc $(CFLAGS) -c linenoise.c
	gcc $(CFLAGS) -c popen.c
	$(CC) $(CPPFLAGS) -c colorprint.cpp
	$(CC) $(CPPFLAGS) -c proc.cpp
	$(CC) $(CPPFLAGS) -c do_shell.cpp

clean:
	rm -f $(OUT) *.o history.txt .gdb_history peda-session-*

check:
	valgrind -v --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(OUT)

help:
	@printf "make clean dep all:			to make clean and all\n"
	@printf "make check				to make check memory by valgrind\n"
