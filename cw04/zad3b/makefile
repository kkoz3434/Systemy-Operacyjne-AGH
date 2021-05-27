CC = gcc



compile:
	$(CC) catcher.c -o catcher
	$(CC) sender.c -o sender
all:
	make compile
	echo " KILL "
	./catcher kill_mode &
	./sender kill_mode 5
	echo " SIGRT" 
	./catcher sigrt_mode &
	./sender sigrt_mode 5

	make clean
clean:
	rm -f catcher sender