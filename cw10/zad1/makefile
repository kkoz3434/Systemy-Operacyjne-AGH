
all:
	make clean
	make server
	make client
	 ./server 6500 socket

client:
	gcc -Wall -g client.c -o client -lpthread -lm

server:
	gcc -Wall -g server.c -o server -lpthread -lm

clean:
	rm -f server client socket
