CC = gcc
CFLAGS = -Og -Wall -Werror -g
FILE = do_shell.c
OUT = myshell

all: dep
	$(CC) $(CFLAGS) *.o -o $(OUT)

dep:
	$(CC) $(CFLAGS) -c parser.[ch]
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f $(OUT) *.o *.gch peda-session-*.txt .gdb_history

info:
	cloc .

help:
	@printf "make clean dep all:                              to make clean and all\n"
	@printf "make info:          to get the number of lines(Will add more features)\n"
