all:
	gcc -c procreader.c -pthread
	gcc -c analyzer.c -pthread
	gcc -c main.c -pthread
	gcc -c doublebuf.c -pthread
	gcc -c ringbuffer.c
	gcc -c printer.c
	gcc main.o procreader.o analyzer.o doublebuf.o ringbuffer.o printer.o -pthread