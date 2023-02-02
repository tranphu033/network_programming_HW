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
#include "lib.h" 

#define PORT 5500
#define BACKLOG 20
#define BUFF_SIZE 1024

node *readData(FILE *f,node *n){
	ElType e;
	while(fscanf(f,"%s %s %d",e.uname,e.pass,&e.status) !=EOF){
	  if (n==NULL) n=chenDau(n,e);
	  else n=chenCuoi(n,e);
	}
	return n;
}

/* Handler process signal*/
void sig_chld(int signo);

/*
* Receive and echo message to client
* [IN] sockfd: socket descriptor that connects to client 	
*/
void process(int sockfd, node *root);

int main(){
	int listen_sock, conn_sock; /* file descriptors */
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	pid_t pid;
	int sin_size;
	FILE *f;
  	node *head=NULL;

	f=fopen("taikhoan.txt","r");
	head=readData(f,head);//load data into server
	fclose(f);

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
	signal(SIGCHLD, sig_chld);

	while(1){
		sin_size=sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock, (struct sockaddr *)&client, (socklen_t*)&sin_size))==-1){
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
			process(conn_sock, head);
			close(conn_sock);					
			exit(0);
		}
		
		/* The parent closes the connected socket since the child handles the new client */
		close(conn_sock);
	}
	close(listen_sock);
	freeList(head);
	return 0;
}

void sig_chld(int signo){
	pid_t pid;
	int stat;
	
	/* Wait the child process terminate */
	while((pid = waitpid(-1, &stat, WNOHANG))>0)
		printf("\nChild %d terminated\n",pid);
}

void process(int sockfd, node *root) {
	char buff[BUFF_SIZE];
	int bytes_sent, bytes_received;
	node *n;
	account acc;
	ElType el;
	FILE *f;

	//nhan thong tin ve username va password tu client
	bytes_received = recv(sockfd, &acc, sizeof(account), 0);
	if (bytes_received < 0)
		perror("\nError: ");
	else if (bytes_received == 0)
		printf("Connection closed.");
	if (bytes_received <= 0) return;
	
	strcpy(el.uname, acc.uname);
	if((n=timkiem(root,el)) == NULL){ //neu username khong ton tai
		strcpy(buff,un_incorr); //un_incorr duoc dinh nghia o file "lib.h"
	}
	else {
		if(n->inf.status == 0) // neu tai khoan bi khoa hoac chua kich hoat
			strcpy(buff, acc_blocked);
		else if(strcmp(n->inf.pass, acc.pass) !=0){ //neu password sai
			strcpy(buff,pass_incorr);
		}
		else strcpy(buff,login_succ); //username va password deu dung
	}
	//gui thong diep bao cho client biet ve tinh trang dang nhap
	bytes_sent = send(sockfd, buff, strlen(buff), 0);
	if (bytes_sent < 0){
		perror("\nError: ");
		return;
	}
	if(strcmp(buff,login_succ) == 0){ //neu username va password deu dung
		while(1){
			//nhan noi dung chat tu client
			bytes_received = recv(sockfd, buff, BUFF_SIZE, 0);
			if (bytes_received < 0)
				perror("\nError: ");
			else if (bytes_received == 0)
				printf("Connection closed.");
			if (bytes_received <= 0) return;
			buff[bytes_received]='\0';

			f=fopen("groupchat.txt","a");
			if(strcmp(buff,"\n") != 0){
				// them noi dung chat moi vao file "groupchat.txt" theo format: "username: noi dung chat"
				fprintf(f,"   <%s>: %s", acc.uname, buff); 
			}
			fclose(f);

			//doc file, gui noi dung doan chat cho client
			f=fopen("groupchat.txt","r");
			while(1){
				if(fgets(buff, 100, f) == NULL){ 
					strcpy(buff,stop_tran); //stop_tran duoc dinh nghia o file "lib.h"
				}				
				bytes_sent = send(sockfd, buff, strlen(buff), 0); //gui noi dung chat
				if (bytes_sent < 0){
					perror("\nError: ");
					return;
				}
				if(strcmp(buff,stop_tran) == 0) break; //ket thuc gui noi dung chat (khi doc het file)

				//nhan thong diep tu client (khong quan tam toi noi dung thong diep nay) truoc khi gui noi dung chat tiep theo:
				bytes_received=recv(sockfd, buff, BUFF_SIZE, 0);
				if (bytes_received < 0)
					perror("\nError: ");
				else if (bytes_received == 0)
					printf("Connection closed.");
				if (bytes_received <= 0) return;
			}
			fclose(f);
		}
	}	
}