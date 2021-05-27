all:
	gcc -Wall -g main.c -o main -pthread

clean:
	rm *_negative.pgm

test:
	./main 2 block body.pgm body_negative.pgm
	./main 6 block body.pgm body_negative.pgm
	./main 12 block body.pgm body_negative.pgm
	./main 18 block body.pgm body_negative.pgm
	./main 2 block barbara.ascii.pgm barbara_negative.pgm
	./main 6 block barbara.ascii.pgm barbara_negative.pgm
	./main 12 block barbara.ascii.pgm barbara_negative.pgm
	./main 18 block barbara.ascii.pgm barbara_negative.pgm
	./main 2 block dla.ascii.pgm dla.ascii_negative.pgm
	./main 6 block dla.ascii.pgm dla.ascii_negative.pgm
	./main 12 block dla.ascii.pgm dla.ascii_negative.pgm
	./main 18 block dla.ascii.pgm dla.ascii_negative.pgm
	./main 2 block glassware_noisy.ascii.pgm glassware_negative.pgm
	./main 6 block glassware_noisy.ascii.pgm glassware_negative.pgm
	./main 12 block glassware_noisy.ascii.pgm glassware_negative.pgm
	./main 18 block glassware_noisy.ascii.pgm glassware_negative.pgm
	./main 2 numbers body.pgm body_negative.pgm
	./main 6 numbers body.pgm body_negative.pgm
	./main 12 numbers body.pgm body_negative.pgm
	./main 18 numbers body.pgm body_negative.pgm
	./main 2 numbers barbara.ascii.pgm barbara_negative.pgm
	./main 6 numbers barbara.ascii.pgm barbara_negative.pgm
	./main 12 numbers barbara.ascii.pgm barbara_negative.pgm
	./main 18 numbers barbara.ascii.pgm barbara_negative.pgm
	./main 2 numbers dla.ascii.pgm dla.ascii_negative.pgm
	./main 6 numbersdla.ascii.pgm dla.ascii_negative.pgm
	./main 12 numbers dla.ascii.pgm dla.ascii_negative.pgm
	./main 18 numbers dla.ascii.pgm dla.ascii_negative.pgm
	./main 2 numbers glassware_noisy.ascii.pgm glassware_negative.pgm
	./main 6 numbers glassware_noisy.ascii.pgm glassware_negative.pgm
	./main 12 numbers glassware_noisy.ascii.pgm glassware_negative.pgm
	./main 18 numbers glassware_noisy.ascii.pgm glassware_negative.pgm




