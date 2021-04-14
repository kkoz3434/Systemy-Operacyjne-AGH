CC = gcc



compile:
	$(CC) catcher.c -o catcher
	$(CC) sender.c -o sender
all:
	make compile
	echo " KILL "
	./catcher queue_mode &
	./sender queue_mode 5

	make clean
clean:
	rm -f catcher sender