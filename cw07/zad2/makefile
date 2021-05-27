all:
	gcc -Wall -g main.c -o main -lpthread -lrt
	gcc -Wall -g pizza_maker.c -o pizza_maker -lpthread -lrt
	gcc -Wall -g driver.c -o driver -lpthread -lrt
	make run

run:
	./main 5 5
