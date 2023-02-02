/*UDP Echo Server*/
#include <stdio.h>          /* These are the usual header files */
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define PORT 5550  /* Port that will be opened */ 
#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
	int server_sock; /* file descriptors */
	char buff[BUFF_SIZE],buff2[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client, client_t; /* client's address information */
	int sin_size;
	//int len;

	//Step 1: Construct a UDP socket
	if ((server_sock=socket(AF_INET, SOCK_DGRAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		exit(0);
	}
	
	//Step 2: Bind address to socket
	server.sin_family = AF_INET;         
	server.sin_port = htons(atoi(argv[1]));   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = INADDR_ANY;  /* INADDR_ANY puts your IP address automatically */   
	bzero(&(server.sin_zero),8); /* zero the rest of the structure */

  
	if(bind(server_sock,(struct sockaddr*)&server,sizeof(struct sockaddr))==-1){ /* calls bind() */
		perror("\nError: ");
		exit(0);
	}     
	/*len = sizeof(client1);
	recvfrom(server_sock, buff, BUFF_SIZE, 0,(struct sockaddr *) &client1, (socklen_t*)&len); //connect with client1
	strcpy(buff,"user1");
	sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *) &client1, sizeof(client1));

	len = sizeof(client2);
	recvfrom(server_sock, buff, BUFF_SIZE, 0,(struct sockaddr *) &client2, (socklen_t*)&len); //connect with client2
	strcpy(buff,"user2");
	sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr *) &client2, sizeof(client2));
	*/

	//Step 3: Communicate with clients
	while(1){
		sin_size=sizeof(struct sockaddr_in);
    	//nhan du lieu tu client 1
		bytes_received = recvfrom(server_sock, buff, BUFF_SIZE-1, 0, (struct sockaddr *) &client, (socklen_t*) &sin_size);
		if (bytes_received < 0)
			perror("\nError: ");
		else{
			buff[bytes_received] = '\0';
			printf("[%s:%d]: %s", inet_ntoa(client.sin_addr), ntohs(client.sin_port), buff);
		}
		if(strcmp(buff,"a1")!=0) {
			strcpy(buff2,buff);
		}
		else {
			client_t=client;
		//gui du lieu sau xu ly cho client 2		
		bytes_sent = sendto(server_sock, buff2, bytes_received, 0, (struct sockaddr *) &client_t, sin_size ); /* send to the client welcome message */
		if (bytes_sent < 0)
			perror("\nError: ");					
	}
	
	close(server_sock);
	return 0;
}
}

/*void processString(char *buff,char *letStr, char *numStr){

}*/
