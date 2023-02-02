CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: client server

client: tcp_client.o
	${CC} tcp_client.o -o client

tcp_client.o: tcp_client.c
	${CC} ${CFLAGS} tcp_client.c

server: tcp_server.o
	${CC} tcp_server.o -o server

tcp_server.o: tcp_server.c
	${CC} ${CFLAGS} tcp_server.c

clean:
	rm -f *.o *~
