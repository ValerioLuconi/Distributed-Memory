CC=g++
CFLAGS=-Wall
LIBS=-lpthread

all: server distmem.o
server: main.o dm.o utility.o block.o
	$(CC) $(CFLAGS) -o server main.o dm.o block.o utility.o $(LIBS)
main.o: dm.h utility.h msg.h
distmem.o: distmem.h utility.h msg.h
dm.o: dm.h
block.o: block.h
utility.o: utility.h

clean:
	@$(RM) *.o server
