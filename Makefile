all:
	gcc -c procreader.c -pthread -g
	gcc -c analyzer.c -pthread -g
	gcc -c main.c -pthread -g
	gcc -c doublebuf.c -pthread -g
	gcc -c ringbuffer.c -g
	gcc -c printer.c -g
	gcc -c threadmanager.c -g
	gcc main.o procreader.o analyzer.o doublebuf.o ringbuffer.o printer.o threadmanager.o -pthread -g