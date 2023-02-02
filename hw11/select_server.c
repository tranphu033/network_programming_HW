#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h> 
#include <sys/time.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include "define.h"

//#define PORT 5500   /* Port that will be opened */ 
#define BACKLOG 20   /* Number of allowed connections */
#define BUFF_SIZE 1024
#define N 26 //tong so ky tu bang chu cai tieng Anh

void ceasar_encrypt(char *str, int key){
    int i,num;
    for(i=0;i<strlen(str);i++){
        if(str[i]>='a' && str[i]<='z'){
            num=str[i]-97;
            str[i]= (char)((num+key) % N) + 97;
        }
        else if(str[i]>='A' && str[i]<='Z'){
            num=str[i]-65;
            str[i]= (char)((num+key) % N) + 65;
        }
    }
}

void ceasar_decrypt(char *str, int key){
    int i,num, tmp;
    for(i=0;i<strlen(str);i++){
        if(str[i]>='a' && str[i]<='z'){
            num=str[i]-97;
            tmp=num-key;
            if(tmp<0) tmp=tmp+N;
            str[i]= (char)(tmp % N) + 97;
        }
        else if(str[i]>='A' && str[i]<='Z'){
            num=str[i]-65;
            tmp=num-key;
            if(tmp<0) tmp=tmp+N;
            str[i]= (char)(tmp % N) + 65;
        }
    }
}

void process(int sockfd, int index); //ham xu ly chinh
/* The recv() wrapper function*/
int receiveData(int s, message *buff, int size, int flags);

/* The send() wrapper function*/
int sendData(int s, message *buff, int size, int flags);

int main(int argc, char *argv[])
{
	int i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready, client[FD_SETSIZE];
	//ssize_t	ret;
	fd_set	readfds, allset;
	//message sendBuff, rcvBuff;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	//message tmp;

	if(argc!=2) {
		printf("Usage: ./server Port_Number\n");
		exit(0);
	}
	//Step 1: Construct a TCP socket to listen connection request
	if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}

	//Step 2: Bind address to socket
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(atoi(argv[1]));

	if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	} 

	//Step 3: Listen request from client
	if(listen(listenfd, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}

	maxfd = listenfd;			/* initialize */
	maxi = -1;				/* index into client[] array */
	for (i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;			/* -1 indicates available entry */
	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);
	
	//Step 4: Communicate with clients
	while (1) {
		readfds = allset;		/* structure assignment */
		nready = select(maxfd+1, &readfds, NULL, NULL, NULL);
		if(nready < 0){
			perror("\nError: ");
			return 0;
		}
		
		if (FD_ISSET(listenfd, &readfds)) {	/* new client connection */
			clilen = sizeof(cliaddr);
			if((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen)) < 0)
				perror("\nError: ");
			else{
				printf("You got a connection from %s\n", inet_ntoa(cliaddr.sin_addr)); /* prints client's IP */
				for (i = 0; i < FD_SETSIZE; i++)
					if (client[i] < 0) {
						client[i] = connfd;	/* save descriptor */
						break;
					}
				if (i == FD_SETSIZE){
					printf("\nToo many clients");
					close(connfd);
				}

				FD_SET(connfd, &allset);	/* add new descriptor to set */
				if (connfd > maxfd)
					maxfd = connfd;		/* for select */
				if (i > maxi)
					maxi = i;		/* max index in client[] array */

				if (--nready <= 0)
					continue;		/* no more readable descriptors */
			}
		}

		for (i = 0; i <= maxi; i++) {	/* check all clients for data */
			if ( (sockfd = client[i]) < 0)
				continue;
			if (FD_ISSET(sockfd, &readfds)) {

				process(sockfd, i);

				FD_CLR(sockfd, &allset);
				close(sockfd);
				client[i] = -1;			

				if (--nready <= 0)
					break;		/* no more readable descriptors */
			}
		}
	}
	
	return 0;
}

void process(int sockfd, int index){
	message rcvBuff, sendBuff;
	int ret;
	char func;
	short key;
	FILE *f1, *f2;
	char buf[BUFF_SIZE], path1[200], path2[200], 
		fol_path[200]; //luu file tam va file ket qua (tam thoi)
	char fname[60];
	strcpy(fol_path,"./server_files/");
	
	//nhan thong tin ve chuc nang va khoa:
	ret = receiveData(sockfd, &rcvBuff, sizeof(message), 0);
	if(ret<=0) return;

	func=rcvBuff.opcode;
	key=atoi(&rcvBuff.payload[0]);

	//nhan du lieu file tu client:
	strcpy(path1,fol_path);
	sprintf(fname,"client%d",index); //ten file tam
	strcat(path1,fname);
	f1=fopen(path1,"w");
	while(1){
		if(receiveData(sockfd, &rcvBuff, sizeof(message), 0) <=0)
			return;
		if(rcvBuff.opcode=='2' && rcvBuff.len==0) break; //hoan tat nhan du lieu file
		else if(rcvBuff.opcode=='2' && rcvBuff.len>0){
			fputs(rcvBuff.payload,f1);
		}
	}
	fclose(f1);
	
	//xu ly du lieu file:
	
	f1=fopen(path1,"r");

	strcpy(path2,fol_path);
	sprintf(fname,"client%d_res",index); //ten file ket qua
	strcat(path2,fname);
	f2=fopen(path2,"w");
	
	while(1){
		if(fgets(buf,BUFF_SIZE,f1) == NULL){
			break;
		}
		else{
			if(func=='0') ceasar_encrypt(buf,key); //ma hoa
			if(func=='1') ceasar_decrypt(buf,key); //giai ma
			fputs(buf,f2);
		}
	}
	fclose(f1); fclose(f2);

	//gui du lieu file ket qua cho client:
	f2=fopen(path2,"r");
	sendBuff.opcode='2';
	while(1){
		if(fgets(buf,BUFF_SIZE,f2) == NULL){
			sendBuff.len=0;
		}else{
			sendBuff.len = strlen(buf);
			strcpy(sendBuff.payload, buf);
		}
		if(send(sockfd, &sendBuff, sizeof(sendBuff), 0) < 0)
			return;
		if(sendBuff.len==0) break; //hoan tat gui du lieu		
	}
	fclose(f2);
	//xoa file tam va file ket qua:
	remove(path1);
	remove(path2);
}

int receiveData(int s, message *buff, int size, int flags){
	int n;
	n = recv(s, buff, size, flags);
	if(n < 0)
		perror("Error: ");
	return n;
}

int sendData(int s, message *buff, int size, int flags){
	int n;
	n = send(s, buff, size, flags);
	if(n < 0)
		perror("Error: ");
	return n;
}

