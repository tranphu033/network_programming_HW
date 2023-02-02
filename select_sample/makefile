CFLAGS = -c -Wall
CC = gcc
LIBS =  -lm 

all: client server

client: tcp_client.o
	${CC} tcp_client.o -o client

tcp_client.o: tcp_client.c
	${CC} ${CFLAGS} tcp_client.c

server: select_server.o
	${CC} select_server.o -o server
	
select_server.o: select_server.c
	${CC} ${CFLAGS} select_server.c

clean:
	rm -f *.o *~
