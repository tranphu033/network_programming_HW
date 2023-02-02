#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>
#include "lib.h" 

#define BUFF_SIZE 1024

int main(int argc, char *argv[]){
	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	int bytes_sent, bytes_received;
	char msg[BUFF_SIZE];
	account acc;

	if(argc!=5) {
		printf("Usage: ./client IPAddress PortNumber username password\n");
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
	strcpy(acc.uname,argv[3]);
	strcpy(acc.pass,argv[4]);
	//gui goi tin kieu "struct account" chua thong tin ve username va password:
	bytes_sent = send(client_sock, &acc, sizeof(acc), 0);
	if(bytes_sent < 0){
		perror("\nError: ");
		close(client_sock);
		return 0;
	}
	//nhan thong diep tra ve tu server de biet tinh trang dang nhap:
	bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
	if (bytes_received < 0)
		perror("\nError: ");
	else if (bytes_received == 0)
		printf("Connection closed.");
	if (bytes_received <= 0){
		close(client_sock);
		return 0;
	}
	buff[bytes_received]='\0';

	if(strcmp(buff,un_incorr) == 0) 
		printf("Tai khoan khong ton tai!\n");
	else if(strcmp(buff,acc_blocked) == 0)
		printf("Tai khoan bi khoa hoac chua kich hoat!\n");
	else if(strcmp(buff,pass_incorr) == 0) 
		printf("Sai mat khau!\n");
	else if(strcmp(buff,login_succ) == 0) {
		printf("Dang nhap thanh cong!\n");
		printf("[Nhap 'exit' de thoat]\n");
		while (1){
			printf("Send: ");
			fgets(msg, BUFF_SIZE, stdin); // nhap noi dung chat
			
			if(strcmp(msg,"exit\n") == 0) break; // thoat chuong trinh

			// gui noi dung chat
			bytes_sent = send(client_sock, msg, strlen(msg), 0);
			if (bytes_sent < 0){
				perror("\nError: ");
				break;
			}
			// hien thi noi dung doan chat
			while (1){
				bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
				if (bytes_received < 0)
					perror("\nError: ");
				else if (bytes_received == 0)
					printf("Connection closed.");
				if (bytes_received <= 0){
					close(client_sock);
					return 0;
				}
				buff[bytes_received] = '\0';
				if (strcmp(buff, stop_tran) == 0)
					break; // ket thuc nhan noi dung chat
				
				printf("%s", buff);// hien thi noi dung chat vua nhan duoc
				
				// gui 1 thong diep de server nhan duoc; 
				// muc dich: tao 1 khoang thoi gian truoc lan gui noi dung chat tiep theo o phia server
				if (send(client_sock, "kkk", strlen("kkk"), 0) < 0){
					perror("\nError: ");
					close(client_sock);
					return 0;
				}
			}
		}
	}
	//Step 4: Close socket
	close(client_sock);
	return 0;
}
