CC = g++
CFLAGS = -Wall -W -Os -g
OUT = myshell2

obj: 
	$(CC) $(CFLAGS) -c linenoise.c
	$(CC) $(CFLAGS) -c do_shell.cpp

all: $(obj)
	$(CC) -o myshell *.o

clean:
	rm -f myshell *.o
