# *****************************************************
# Giulianno Raphael Sbrugnera RA: 408093
# Rafael Paschoal Giordano    RA: 408298

# *****************************************************
# Makefile for executable

# *****************************************************
# Parameters to control Makefile operation

CC = gcc
CFLAGS = -Wall

# ****************************************************
# Entries to bring the executable up to date

all: teste clean

teste: main.o 408093_408298_ED2_T01.o
	$(CC) $(CFLAGS) -o teste main.o 408093_408298_ED2_T01.o

main.o: main.c 408093_408298_ED2_T01.h
	$(CC) $(CFLAGS) -c main.c

408093_408298_ED2_T01.o: 408093_408298_ED2_T01.c 408093_408298_ED2_T01.h
	$(CC) $(CFLAGS) -c 408093_408298_ED2_T01.c

clean:
	rm -f *.o

uninstall:
	rm -f teste	*~ *.idx *.dat
