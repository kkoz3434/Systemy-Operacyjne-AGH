CC = gcc -Wall -g

compile: zad

zad: main.c
	$(CC) main.c -o zad1
	$(CC) child.c -o child

all:
	make compile
	./zad1 handle
	./zad1  ignore
	./zad1 mask
	./zad1 pending
	make clean

clean:
	rm -f zad1 child