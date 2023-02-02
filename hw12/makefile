CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: client server

client: tcp_client.o
	${CC} tcp_client.o -o client

tcp_client.o: tcp_client.c
	${CC} ${CFLAGS} tcp_client.c

server: tcp_server_nbio.o
	${CC} tcp_server_nbio.o -o server
	
tcp_server_nbio.o: tcp_server_nbio.c
	${CC} ${CFLAGS} tcp_server_nbio.c

clean:
	rm -f *.o *~
