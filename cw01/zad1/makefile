CC = gcc -Wall -std=c11 -g

all:
	make static shared

static:
	$(CC) -c lib.c
	ar rcs library.a lib.o

shared:
	$(CC) -fPIC -c lib.c
	$(CC) -shared -fPIC -o lib.so lib.o


clean:
	rm -f *.o *.a *.so