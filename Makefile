#CC=clang
#CFLAGS=-g -Weverything -Wno-padded -Wno-declaration-after-statement -Wno-disabled-macro-expansion
CFLAGS=-g

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

.PHONY: clean
clean:
	rm $(ODIR)/*.o
