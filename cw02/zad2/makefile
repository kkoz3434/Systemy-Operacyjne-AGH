CC =  gcc -Wall -g

all:
	make compile
	./zad2 a long1.txt >> pomiar_zad_2.txt

clean:
	rm -f *.out
	rm -f *.o
	rm -f main_a
	rm -f main_b
	rm -f *.a
compile:
	$(CC) -o zad2 main.c
