CC = gcc



compile:
	$(CC) catcher.c -o catcher
	$(CC) sender.c -o sender
all:
	make compile
	echo " KILL "
	./catcher kill_mode &
	./sender kill_mode 10000
	echo " SIGRT "
	./catcher sigrt_mode &
	./sender sigrt_mode 10000
	echo " QUEUE "
	./catcher queue_mode &
	./sender queue_mode 10000
	make clean
clean:
	rm -f catcher sender