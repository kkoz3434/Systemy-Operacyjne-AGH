
all:
	gcc -Wall -g main.c -o main
	make run
	make clean

run:
	./main date
	./main sender
	./main root@localhost test_topic text

clean:
	rm -f main