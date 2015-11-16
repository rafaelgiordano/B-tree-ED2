# *****************************************************
# Giulianno Raphael Sbrugnera RA: 408093
# Rafael Paschoal Giordano    RA: 408298

# *****************************************************
# Makefile for executable

# *****************************************************
# Parameters to control Makefile operation

CC = gcc
CFLAGS = -Wall
LDFLAGS = -lm
# alternativa ao LDFLAGS: LNKLIB.

# ****************************************************
# Entries to bring the executable up to date

all: teste clean

teste: main.o 408093_408298_ED2_T02.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o teste main.o 408093_408298_ED2_T02.o

main.o: main.c 408093_408298_ED2_T02.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c main.c

408093_408298_ED2_T02.o: 408093_408298_ED2_T02.c 408093_408298_ED2_T02.h
	$(CC) $(CFLAGS) $(LDFLAGS) -c 408093_408298_ED2_T02.c

clean:
	rm -f *.o

uninstall:
	rm -f teste	*~ *.dat
