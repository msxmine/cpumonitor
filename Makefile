CC=clang
CFLAGS=-g -Weverything -Wno-padded -Wno-declaration-after-statement -Wno-disabled-macro-expansion

ODIR=obj
LIBS=-pthread

_OBJ = main.o procreader.o analyzer.o doublebuf.o ringbuffer.o printer.o threadmanager.o logger.o timeutils.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

cpumonitor: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean
clean:
	rm ./$(ODIR)/*.o
