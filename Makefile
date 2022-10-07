#CC=clang
#CFLAGS=-g -Weverything -Wno-padded -Wno-declaration-after-statement -Wno-disabled-macro-expansion -Wno-format-nonliteral
#CC=gcc
#CFLAGS=-g -Wall -Wextra

ODIR=obj
LIBS=-pthread

_OBJ = procreader.o analyzer.o doublebuf.o ringbuffer.o printer.o threadmanager.o logger.o timeutils.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

cpumonitor: $(ODIR)/main.o $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

test: $(ODIR)/test.o $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

all: cpumonitor test

.PHONY: clean
clean:
	rm $(ODIR)/*.o
