CC = g++
CFLAGS = -Wall -W -Os -g
CPPFLAGS = -Wall -W -Os -g -std=c++14
OUT = myshell

obj: 
	gcc $(CFLAGS) -c linenoise.c
	gcc $(CFLAGS) -c popen.c
	$(CC) $(CPPFLAGS) -c proc.cpp
	$(CC) $(CPPFLAGS) -c do_shell.cpp

all: obj
	$(CC) -o $(OUT) *.o

clean:
	rm -f $(OUT) *.o history.txt

check:
	valgrind -v --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(OUT)