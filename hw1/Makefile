CC = gcc
CFLAGS = -O1 -g -Wall -Werror -o
FILE = do_shell.c
OUT = myshell

all:
	$(CC) ${FILE} ${CFLAGS} ${OUT}
clean:
	rm -rf ${OUT}
check: all
	./${OUT}
