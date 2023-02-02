#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include "lib.h"

void printOutput(char *mess){
	int m=atoi(mess);
	switch(m){
		case 1: printf("Account is not found!\n"); break;
		case 2: printf("Account is blocked or inactive!\n"); break;
		case 3: printf("Password is not correct. Please try again!\n"); break;
		case 4: printf("Account is blocked!\n"); break;
		case 5: printf("Login is successful!\n"); break;
		default: break;
	}
}

int main(int argc, char* argv[]){
	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	int bytes_received;
	char uname[BUFF_SIZE], pass[BUFF_SIZE];
	int uname_len,pass_len;
	char sign_out[30];

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
	while(1){
		//send username
		memset(buff,'\0',strlen(buff)+1);
		memset(uname,'\0',(strlen(uname)+1));
		printf("Enter your username: ");
		fgets(uname, 30, stdin);	
		uname_len = strlen(uname);

		
		if(send(client_sock, uname, uname_len, 0) <= 0){
			printf("\nConnection closed!\n");
			break;
		}
		if((bytes_received=recv(client_sock, buff, BUFF_SIZE-1, 0)) <= 0){
			printf("\nError!Cannot receive data from sever!\n");
			break;
		}
		buff[bytes_received]='\0';
		printOutput(buff);

		if(strcmp(buff,"ok") == 0){
		//send password
			while(1){
				memset(pass,'\0',(strlen(pass)+1));
				printf("Enter your password: ");
				fgets(pass, 50, stdin);
				pass_len = strlen(pass);

				if (send(client_sock, pass, pass_len, 0) <= 0){
					printf("\nConnection closed!\n");
					break;
				}
				if ((bytes_received=recv(client_sock, buff, BUFF_SIZE - 1, 0)) <= 0){
					printf("\nError!Cannot receive data from sever!\n");
					break;
				}
				buff[bytes_received] = '\0';
				printOutput(buff);
				if(strcmp(buff,pass_incor) == 0) continue; //re-enter password
				else break;
				}
				//if login sucess-> sign out?
				if (strcmp(buff, login_sucess) == 0){
					while (1){
						printf("Do you want to sign out? (Enter [bye] to sign out) ");
						fgets(sign_out, 30, stdin);
						sign_out[strlen(sign_out)-1] = '\0';
						
						if (strcmp(sign_out,"bye") ==0){
							if (send(client_sock, "bye", strlen("bye"), 0) <= 0){
								printf("\nConnection closed!\n");
								break;
							}
							if ((bytes_received = recv(client_sock, buff, BUFF_SIZE - 1, 0)) <= 0){
								printf("\nError!Cannot receive data from sever!\n");
								break;
							}
							buff[bytes_received] = '\0';

							if (strcmp(buff, "sign_out") == 0){
								printf("Goodbye %s", uname);
								break;
							}
							else continue;
						}
					}
				}
		}
	}
	
	//Step 4: Close socket
	close(client_sock);
	return 0;
}
