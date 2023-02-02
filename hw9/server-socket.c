#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

//Remember to use -pthread when compiling this server's source code
void *connection_handler(void *);

int main()
{
    //char server_message[100] = "Hello from Server!!\n";
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
	
    if (server_socket==-1){
	perror("Socket initialisation failed");
	exit(EXIT_FAILURE);
	}
    else
	printf("Server socket created successfully\n");

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5500);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    //bind the socket to the specified IP addr and port
    if (bind(server_socket, (struct sockaddr*) &server_addr, sizeof(server_addr))!=0){
	printf("socket bind failed...\n"); 
        exit(0);
	}
    else
		printf("Socket successfully binded..\n"); 
    
    if (listen(server_socket, 3)!=0){
		printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    
    int no_threads=0;
    pthread_t threads[3];
    while (no_threads<3){
		printf("Listening...\n");
		int client_socket = accept(server_socket, NULL, NULL);
		puts("Connection accepted");
		if (pthread_create(&threads[no_threads], NULL, connection_handler, &client_socket) < 0){
			perror("Could not create thread");
			return 1;
		}
		if (client_socket < 0){
			printf("server acccept failed...\n");
			exit(0);
		}
		else
			printf("Server acccept the client...\n");
		puts("Handler assigned");
		no_threads++;
	}
	int k=0;
    for (k=0;k<3;k++){
	pthread_join(threads[k],NULL);
}
    close(server_socket);   
    return 0;
}

void *connection_handler(void *client_socket){
	int socket = *(int*) client_socket;
	int read_len, i;
	int send_status;
    char client_message[256];
	while(1){
		//nhan message tu client
		read_len=recv(socket,client_message, 100,0);
		if(read_len <= 0){
			perror("\nError: "); break;
		}
		//end of string marker
		client_message[read_len] = '\0';
		
		//xu ly message nhan duoc:
		if(strcmp(client_message,"q")==0 || strcmp(client_message,"Q")==0){ //dau hieu ket thuc chg trinh phia client
			break;
		}
		else{			
			for (i = 0; i < strlen(client_message); i++){
				if (!isalpha((int)client_message[i])){
					strcpy(client_message, "Wrong text format!");
					break;
				}
				else{
					if(client_message[i] >= 'a' && client_message[i] <= 'z')
						client_message[i] = client_message[i] - 32; //doi chu cai thuong thanh hoa
				}
			}
		}
		//gui phan hoi ve client
		send_status=send(socket , client_message , strlen(client_message),0);
		if(send_status < 0){
			perror("\nError: "); break;
		}	
	}	
	return 0;
}

