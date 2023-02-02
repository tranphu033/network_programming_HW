/*UDP Echo Client*/
#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define SERV_PORT 5550
#define SERV_IP "127.0.0.1"
#define BUFF_SIZE 1024

int main(int argc, char *argv[]){
	int client_sock;
	char buff[BUFF_SIZE], user[10];
	struct sockaddr_in server_addr;
	int bytes_sent, bytes_received, sin_size;
	int cli;
	
	//Step 1: Construct a UDP socket
	if ((client_sock=socket(AF_INET, SOCK_DGRAM, 0)) < 0 ){  /* calls socket() */
		perror("\nError: ");
		exit(0);
	}

	//Step 2: Define the address of the server
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	
	
	/*int len = sizeof(server_addr);
	strcpy(buff, "Hello");
	sendto(client_sock, buff, strlen(buff), 0,(struct sockaddr *) &server_addr, len); //send message to server
	recvfrom(client_sock, user, BUFF_SIZE, 0,(struct sockaddr *) &server_addr, (socklen_t*)&len);
	*/
	//Step 3: Communicate with server
	//printf("\nInsert string to send:");
	
	printf("client?:");
	scanf("%d",&cli);
	while(getchar()!='\n');
	if(cli==1){
		printf("Nhap chuoi vao:");
	memset(buff,'\0',(strlen(buff)+1));
	fgets(buff, BUFF_SIZE, stdin);
	//strcpy(buff,"ngoc");
	sin_size = sizeof(struct sockaddr);
	
	//client 1 gui du lieu len server
	bytes_sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *) &server_addr, sin_size);
	if(bytes_sent < 0){
		perror("Error: ");
		close(client_sock);
		return 0;
	}
	}
	
	else if(cli==2){
		printf("Nhap chuoi dummy:");
	memset(buff,'\0',(strlen(buff)+1));
	fgets(buff, BUFF_SIZE, stdin);
	//client 2 gui va nhan du lieu tu server
	bytes_sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *) &server_addr, sin_size);
	if(bytes_sent < 0){
		perror("Error: ");
		close(client_sock);
		return 0;
	}
	bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *) &server_addr, (socklen_t*)&sin_size);
	if(bytes_received < 0){
		perror("Error: ");
		close(client_sock);
		return 0;
	}
	buff[bytes_received] = '\0';
	printf("Reply from server: %s", buff);
	}
	close(client_sock);
	return 0;
}
