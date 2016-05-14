CC=g++
CFLAGS=-Wall
SRC=../src

all: countspace countword

countspace: countspace.o $(SRC)/utility.o $(SRC)/distmem.o
	$(CC) $(CFLAGS) -o countspace countspace.o $(SRC)/distmem.o $(SRC)/utility.o
countspace.o: $(SRC)/distmem.h

countword: countword.o $(SRC)/utility.o $(SRC)/distmem.o
	$(CC) $(CFLAGS) -o countword countword.o $(SRC)/distmem.o $(SRC)/utility.o
countword.o: $(SRC)/distmem.h

clean:
	@$(RM) *.o countword countspace
