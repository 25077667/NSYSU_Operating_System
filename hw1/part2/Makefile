CC = g++
CFLAGS = -Wall -W -Os -g
OUT = myshell2

obj: 
	gcc $(CFLAGS) -c linenoise.c
	$(CC) $(CFLAGS) -c proc.cpp
	$(CC) $(CFLAGS) -c do_shell.cpp

all: obj
	$(CC) -o myshell *.o

clean:
	rm -f myshell *.o *.gch

check:
	valgrind -v --tool=memcheck --leak-check=full --show-leak-kinds=all ./myshell