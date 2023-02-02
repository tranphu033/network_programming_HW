#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include "lib.h"

#define not_found "0"
#define found "1"
#define finish "2"

#define BACKLOG 10   /* Number of allowed connections */
//#define BUFF_SIZE 1024

node *readData(FILE *f,node *n){
	ElType e;
	while(fscanf(f,"%s %s",e.fname,e.path) !=EOF){
	  if (n==NULL) n=chenDau(n,e);
	  else n=chenCuoi(n,e);
	}
	return n;
}

int main(int argc, char* argv[]){

	int listen_sock, conn_sock; /* file descriptors */
	char fname[BUFF_SIZE], buff[BUFF_SIZE];
	char msg[20];
	int bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	int sin_size,k;
	FILE *f, *f1;
	node *head=NULL,*n;
	ElType el; 
	char db_path[]="/home/tranphu/image_ser/image_path.txt"; //duong dan toi file du lieu (chua ten va duong dan anh) phia server
															 //tranphu la ten user trong ubuntu
	if(argc!=2) {
		printf("Usage: ./server PortNumber\n");
		exit(0);
	}
	
	if((f=fopen(db_path,"r")) == NULL){
		printf("Can't open the data file! \nMaybe file path is incorrect or file is not exist.\n");
		exit(0); 
	}
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
	sin_size = sizeof(struct sockaddr_in);
	//Step 4: Communicate with client
	while(1){
		//accept request
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, (socklen_t *)&sin_size)) == -1) 
			perror("\nError: ");
  
		printf("\nYou got a connection from %s", inet_ntoa(client.sin_addr) ); /* prints client's IP */
		
		//start conversation
		bytes_received = recv(conn_sock, fname, BUFF_SIZE-1, 0); //blocking
		if (bytes_received <= 0){
			printf("\nConnection closed 1.");
			close(conn_sock);
			continue;
		}else{
			fname[bytes_received] = '\0';
			printf("\nReceive: [%s] ", fname);
		}

		// search the image file
		strcpy(el.fname, fname);
		if ((n = timkiem(head, el)) == NULL) strcpy(msg, not_found);
		else strcpy(msg, found);

		//send message to client (file is found or not found)
		if (send(conn_sock, msg, 1, 0) <= 0){
			printf("\nConnection closed.");
			close(conn_sock);
			continue;
		}

		if (strcmp(msg, not_found) == 0){ //if file is not found
			printf("\nConnection closed.");
			close(conn_sock);
			continue;
		} 
		else if (strcmp(msg, found) == 0){ //if file is found
			f1 = fopen(n->inf.path, "rb");
			//send file to client
			while (1){
				k = fread(buff, 1, BUFF_SIZE, f1);
				
				if (k != BUFF_SIZE)	{strcpy(buff, finish);k=strlen(finish);}
				if (send(conn_sock, buff, k, 0) <= 0){
					// printf("\nConnection closed.");
					break;
				}
				if (strcmp(buff, finish) == 0) break;
			}
			fclose(f1);
		}
		// end conversation
		printf("\nConnection closed.");
		close(conn_sock);
	}

	close(listen_sock);
	freeList(head);
	return 0;
}
