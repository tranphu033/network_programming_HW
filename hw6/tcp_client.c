#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#define not_found "0"
#define found "1"
#define finish "2"
#define BUFF_SIZE 1024

int check_file_format(char *fname){
	char type[6];
	int type_count=0;
	int i=strlen(fname)-1;
	do{
		i--;
	}
	while(fname[i]!='.' && i>0);
	if(i == 0) return 0;
	for(int j=i+1;j<=strlen(fname);j++){
		type[type_count]=fname[j];
		type_count++;
	}
	//check
	if(strcmp(type,"png") == 0 || strcmp(type,"jpg") == 0 || strcmp(type,"bmp") == 0)
	return 1;
	return 0;
}

int main(int argc, char* argv[]){
	int client_sock;
	char buff[BUFF_SIZE],fname[BUFF_SIZE];	
	struct sockaddr_in server_addr; /* server's address information */
	int bytes_received;
	FILE *f, *fc;
	char path[]="/home/tranphu/image/"; //duong dan toi thu muc luu tru anh phia client
										//tranphu la ten user trong ubuntu
	if(argc!=4) {
		printf("Usage: /client IPAddress PortNumber FileName\n");
		exit(0);
	}
	
	strcpy(fname,argv[3]);
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
	strcat(path, fname);
	//check file format
	if (check_file_format(fname) == 0){
		printf("Error: Wrong File Format!\n");
		close(client_sock);
		return 0;
	}
	//check file exists
	if ((fc = fopen(path, "rb")) != NULL){
		printf("File Already Exists!\n");
		fclose(fc);
		close(client_sock);
		return 0;
	}else {
		//send filename to server
		if(send(client_sock, fname, strlen(fname), 0) <= 0){
			printf("\nConnection closed!\n");
			close(client_sock);
			return 0;
		}

		// receive reply from server

		bytes_received = recv(client_sock, buff, 1, 0);
		if (bytes_received <= 0){
			printf("\nError!Cannot receive data from sever!\n");
			close(client_sock);
			return 0;
		}
		buff[bytes_received] = '\0';

		if (strcmp(buff, not_found) == 0){ //if file is not found
			printf("Error: File Not Found!\n");
			close(client_sock);
			return 0;
		}
		if (strcmp(buff, found) == 0){ //if file is found
			f = fopen(path, "wb");
			//download file
			while(1){
				bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
				if (bytes_received <= 0){
					printf("\nError!Cannot receive data from sever!\n");
					close(client_sock);
					return 0;
				}
				buff[bytes_received] = '\0';

				if (strcmp(buff, finish) == 0){
					printf("File Transfer Is Completed!\n");
					break;
				}
				else fwrite(buff, 1, bytes_received, f);
			}
			fclose(f);
		}
	}
	// Step 4: Close socket
	close(client_sock);
	return 0;
}
