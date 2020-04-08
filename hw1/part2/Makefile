CC = g++
CFLAGS = -Wall -W -Os -g -std=c++14
OUT = myshell

obj: 
	gcc $(CFLAGS) -c linenoise.c
	$(CC) $(CFLAGS) -c proc.cpp
	$(CC) $(CFLAGS) -c do_shell.cpp

all: obj
	$(CC) -o $(OUT) *.o

clean:
	rm -f $(OUT) *.o *.gch

check:
	valgrind -v --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(OUT)