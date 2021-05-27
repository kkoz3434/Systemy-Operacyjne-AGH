CC = gcc -g -Wall

static:
	$(CC) -c lib.c
	ar rcs lib.a lib.o

shared:
	$(CC) -fPIC -c lib.c
	$(CC) -shared -fPIC -o lib.so lib.o

clean:
	rm -f *.o *.a *.so zad2

m_static:
	make static compile_main
	$(CC) main.o lib.a -o zad2

m_shared:
	make shared
	$(CC) -Wl,-rpath=. -o zad2 main.c lib.so

compile_main:
	$(CC) -c main.c

make_test_merge_long_most:
	./zad2 merge_files long1.txt long1.txt long1.txt  long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt  long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt  long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt  long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt

make_test_merge_long_more:
	./zad2 merge_files long1.txt long1.txt long1.txt  long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt  long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt  long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt  long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt

make_test_merge_long:
	./zad2 merge_files long1.txt long1.txt long1.txt  long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt long1.txt  long1.txt long1.txt long1.txt long1.txt long1.txt


make_test_merge_medium_most:
	./zad2 merge_files medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt

make_test_merge_medium_more:
	./zad2 merge_files medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt


make_test_merge_medium:
	./zad2 merge_files medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt medium1.txt

make_test_merge_short_most:
	./zad2 merge_files short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt

make_test_merge_short_more:
	./zad2 merge_files short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt

make_test_merge_short:
	./zad2 merge_files short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt short1.txt

all:
	echo "\n ## STATIC ## "
	make m_static
	echo "\n\n Short files (100 / 500/ 2000)"
	make make_test_merge_short
	make make_test_merge_short_more
	make make_test_merge_short_most
	echo "\n\n Medium files (100 / 500/ 2000)"
	make make_test_merge_medium
	make make_test_merge_medium_more
	make make_test_merge_medium_most
	echo "\n\n Long files (100 / 500/ 2000)"
	make make_test_merge_long
	make make_test_merge_long_more
	make make_test_merge_long_most

	echo "\n ## SHARED ## "
	make m_shared
	echo "\n\n Short files (100 / 500/ 2000)"
	make make_test_merge_short
	make make_test_merge_short_more
	make make_test_merge_short_most
	echo "\n\n Medium files (100 / 500/ 2000)"
	make make_test_merge_medium
	make make_test_merge_medium_more
	make make_test_merge_medium_most
	echo "\n\n Long files (100 / 500/ 2000)"
	make make_test_merge_long
	make make_test_merge_long_more
	make make_test_merge_long_most
