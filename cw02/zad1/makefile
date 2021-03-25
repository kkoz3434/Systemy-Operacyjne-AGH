CC =  gcc -Wall -g

all:
	make compile
	./zad1 long1.txt long1.txt >> pomiar_zad_1.txt

clean:
	rm -f *.out
	rm -f *.o
	rm -f *.a
	rm -f zad1

compile:
	$(CC) -o zad1 main.c
