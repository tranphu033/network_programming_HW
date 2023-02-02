#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/wait.h>
#include <errno.h>
#include <arpa/inet.h>

#define PORT 5500
#define BACKLOG 20
#define BUFF_SIZE 1024
#define finish "1"

/* Handler process signal*/
void sig_chld(int signo);

/*
* Receive and echo message to client
* [IN] sockfd: socket descriptor that connects to client 	
*/
void process(int sockfd);

int main(){
	
	int listen_sock, conn_sock; /* file descriptors */
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	pid_t pid;
	int sin_size;

	if ((listen_sock=socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		printf("socket() error\n");
		return 0;
	}
	
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(PORT);
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   

	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ 
		perror("\nError: ");
		return 0;
	}     

	if(listen(listen_sock, BACKLOG) == -1){  
		perror("\nError: ");
		return 0;
	}
	
	/* Establish a signal handler to catch SIGCHLD */
	signal(SIGCHLD, sig_chld);//

	while(1){
		sin_size=sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client,(socklen_t*)&sin_size))==-1){
			if (errno == EINTR)
				continue;
			else{
				perror("\nError: ");			
				return 0;
			}
		}
		
		/* For each client, fork spawns a child, and the child handles the new client */
		pid = fork();
		
		/* fork() is called in child process */
		if(pid  == 0){
			close(listen_sock);
			printf("You got a connection from %s\n", inet_ntoa(client.sin_addr)); /* prints client's IP */
			process(conn_sock);					
			exit(0);
		}
		
		/* The parent closes the connected socket since the child handles the new client */
		close(conn_sock);
	}
	close(listen_sock);
	return 0;
}

void sig_chld(int signo){
	pid_t pid;
	int stat;
	
	/* Wait the child process terminate */
	while((pid = waitpid(-1, &stat, WNOHANG))>0)
		printf("\nChild %d terminated\n",pid);
}

void process(int sockfd) {
	char buff[BUFF_SIZE], fname[BUFF_SIZE];
	int bytes_received, i, k=0;
	char c;

	//--xu ly ten file--
	bytes_received = recv(sockfd, fname, BUFF_SIZE, 0);
	if (bytes_received <= 0){
		perror("\nError: ");
		close(sockfd);
		return;
	}
	fname[bytes_received] = '\0';

	//chuyen ten file thanh chu hoa
	for(i=strlen(fname)-1; i>=0; i--){
		if(k == 1){
			if ('a' <= fname[i] && fname[i] <= 'z')
				fname[i] = fname[i] - 32;
		}
		if(fname[i] == '.') k=1;
	}

	if(send(sockfd, fname, strlen(fname), 0) < 0){
		perror("\nError: ");
		close(sockfd);
		return;
	}

	//--xu ly phan van ban--
		while (1){
			bytes_received = recv(sockfd, buff, BUFF_SIZE, 0);
			if (bytes_received <= 0){
			perror("\nError: ");
			break;
			}
			buff[bytes_received] = '\0';

			if (strcmp(buff, finish) == 0){
			break;
			}else{
			c = buff[0];
			// chuyen chu thuong thanh chu hoa
			if ('a' <= c && c <= 'z'){
				c = c - 32;
				buff[0] = c;
				buff[1] = '\0';
			}
			if (send(sockfd, buff, 1, 0) < 0){
				perror("\nError: ");
				close(sockfd);
			}
			}
		}
		close(sockfd);
}