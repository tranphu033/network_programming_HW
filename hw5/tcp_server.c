#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "lib.h"

#define BACKLOG 3  /* Number of allowed connections */
//#define BUFF_SIZE 1024

node *readData(FILE *f,node *n){
	ElType e;
	while(fscanf(f,"%s %s %d",e.uname,e.pass,&e.status) !=EOF){
	  if (n==NULL) n=chenDau(n,e);
	  else n=chenCuoi(n,e);
	}
	return n;
}
void writeData(FILE *f,node *n){
    node *tmp;

    for (tmp=n; tmp!=NULL; tmp=tmp->next) {
        fprintf(f,"%s %s %d\n",tmp->inf.uname,tmp->inf.pass,tmp->inf.status);
    } 
}

int main(int argc, char* argv[])
{
 
	int listen_sock, conn_sock; /* file descriptors */
	char buff[BUFF_SIZE], uname[BUFF_SIZE], pass[BUFF_SIZE];
	int bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size,i=0;
	FILE *f;
  	node *head=NULL,*n;
	ElType el;

	f=fopen("account.txt","r");
	head=readData(f,head);//load data into server
	fclose(f);
	
	//Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(atoi(argv[1]));   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   
	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	}     
	
	//Step 3: Listen request from client
	if(listen(listen_sock, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 4: Communicate with client
	while(1){
		//accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client,(socklen_t *) &sin_size)) == -1) 
			perror("\nError: ");
  
		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); /* prints client's IP */
		
		//start conversation
		while(1){
			//receives username from client
			bytes_received = recv(conn_sock, uname, BUFF_SIZE-1, 0); //blocking
			if (bytes_received <= 0){
				printf("\nConnection closed");
				break;
			}else{
				uname[bytes_received-1] = '\0';
				printf("\nReceive: [%s] ", uname);
			}
			//process username
			strcpy(el.uname,uname);
			if((n=timkiem(head,el)) == NULL) strcpy(buff,not_found);
			else{
				if(n->inf.status == 0) strcpy(buff,acc_blocked);
				else strcpy(buff,"ok");
			}

			if(send(conn_sock, buff, strlen(buff), 0) <= 0){ //send message to server
				printf("\nConnection closed");
				break;
			}
			if(strcmp(buff,not_found) == 0) continue;
			if(strcmp(buff,acc_blocked) ==0) continue;
			
			while(1){
				//receive password from client
				bytes_received = recv(conn_sock, pass, BUFF_SIZE - 1, 0); // blocking
				if (bytes_received <= 0){
					printf("\nConnection closed");
					break;
				}else{
					pass[bytes_received - 1] = '\0';
					printf("\nReceive: [%s] ", pass);
				}
				//process password
				if(strcmp(n->inf.pass, pass) != 0) {
					i++;
					if(i<3) strcpy(buff,pass_incor);
					else if(i==3) {
						strcpy(buff,block);
						n->inf.status=0;
						i=0;
						//update status into data file
						f=fopen("account.txt","w");
						writeData(f,head);
						fclose(f);
					}
				}
				else {
					strcpy(buff,login_sucess);
				}

				if (send(conn_sock, buff, strlen(buff), 0) <= 0){ //send message to server
					printf("\nConnection closed");
					break;
				}
				if(strcmp(buff,block) == 0) break;
				else if(strcmp(buff,login_sucess) == 0) break;
				}

				//process "sign out" request after login
				if(strcmp(buff,login_sucess) == 0){
					while(1){
					bytes_received = recv(conn_sock, buff, BUFF_SIZE - 1, 0);
					if (bytes_received <= 0)
					{
						printf("\nConnection closed");
						break;
					}
					else
					{
						buff[bytes_received] = '\0';
						printf("\nReceive: [%s] ", buff);
					}
					if(strcmp(buff,"bye") == 0){
						strcpy(buff, "sign_out");
					}
					if (send(conn_sock, buff, strlen(buff), 0) <= 0){
						printf("\nConnection closed");
						break;
					}
					if(strcmp(buff,"sign_out") == 0) {
						break;
					}
					else continue;
					}
				}
		}//end conversation
		close(conn_sock);	
	}
	close(listen_sock);
	freeList(head);
	return 0;
}
