CC = gcc
CFLAGS = -Wall -W -Og -g -std=gnu99
CPPFLAGS = -Wall -W -Og -g -std=c++14 
OUT = part2.elf

dep: 
	$(CC) $(CFLAGS) -c mm.c
	# g++ $(CPPFLAGS) test.cpp -fPIC -rdynamic -shared -Wl,-soname,libtest.so -o libtest.so
	$(CC) $(CFLAGS) -c test.c
	$(CC) $(CFLAGS) -c main.c

all: dep
	#g++ $(CPPFLAGS) *.o -o $(OUT)
	$(CC) $(CFLAGS) *.o -o $(OUT)

clean:
	rm -f $(OUT) *.o .gdb_history peda-session-* *.so

check:
	valgrind -v --tool=memcheck --leak-check=full --show-leak-kinds=all ./$(OUT)