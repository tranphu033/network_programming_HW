#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "define.h"

//#define SERVER_ADDR "127.0.0.1"
//#define SERVER_PORT 5500
#define BUFF_SIZE 1024

int main(int argc, char *argv[]){
	int client_sock;
	//char buff[BUFF_SIZE + 1];
	struct sockaddr_in server_addr; /* server's address information */
	//int msg_len, bytes_sent, bytes_received;
	message msg;
	
	if(argc!=3) {
		printf("Usage: ./client IPAddress PortNumber\n");
		exit(0);
	}
	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}
		
	//Step 4: Communicate with server			
	char func;
	short key;
	char fname[60], 
	res_fpath[200]; //duong dan luu file ket qua nhan tu server
	char buf[BUFF_SIZE];
	FILE *f;

	do{
	printf("Chon ma hoa(0) hoac giai ma(1):");
	scanf("%c",&func);
	if(func!='0' && func!='1') {
		printf("Khong co lua chon nay!\n");
		while(getchar()!='\n');
	}
	}while(func!='0' && func!='1');

	printf("Nhap khoa:"); scanf("%hd",&key);
	while(getchar()!='\n');
	
	printf("Nhap ten file:");
	fgets(fname,60,stdin);		
	fname[strlen(fname)-1]='\0';

	msg.opcode=func;
	sprintf(msg.payload,"%d",key);
	
	//gui thong tin ve chuc nang va khoa len server:
	if(send(client_sock, &msg, sizeof(msg), 0) < 0){
		perror("\nError: ");
		close(client_sock);
		return 0;
	}

	//gui du lieu file len server:
	f=fopen(fname,"r");
	if(f==NULL) {
		printf("Khong the mo file!\n");
		close(client_sock);
		return 0;
	}
	msg.opcode='2';
	while(1){
		if(fgets(buf,BUFF_SIZE,f) == NULL){
			msg.len=0;
		}else{
			msg.len = strlen(buf);
			strcpy(msg.payload, buf);
		}
		if(send(client_sock, &msg, sizeof(msg), 0) < 0){
			perror("\nError: ");
			close(client_sock);
			return 0;
		}
		if(msg.len==0) break; //hoan tat gui du lieu
	}
	fclose(f);

	//nhan file ket qua tu server:
	strcpy(res_fpath,"./result_files/");
	strcat(res_fpath, fname);
	f=fopen(res_fpath,"w");
	while(1){
		if(recv(client_sock, &msg, sizeof(message), 0) <= 0){
			perror("\nError: ");
			close(client_sock);
			return 0;
		}
		if(msg.opcode=='2' && msg.len==0) break; //hoan tat nhan du lieu file
		else if(msg.opcode=='2' && msg.len>0){
			fputs(msg.payload,f);
		}
	}
	fclose(f);
	if(func=='0') printf("->Ma hoa file thanh cong!\n");
	else if(func=='1') printf("->Giai ma file thanh cong!\n");
	printf("  File ket qua da duoc luu trong folder 'result_files'\n");
	
	//Step 4: Close socket
	close(client_sock);
	return 0;
}
