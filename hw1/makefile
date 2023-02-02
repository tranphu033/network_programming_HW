CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: run

run: account.o dslk.o
	${CC} account.o dslk.o -o run

account.o: account.c
	${CC} ${CFLAGS} account.c

dslk.o: dslk.c
	${CC} ${CFLAGS} dslk.c

clean:
	rm -f *.o *~