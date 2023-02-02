#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <unistd.h>

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 5500
#define BUFF_SIZE 1024
#define finish "1"

int check_file_format(char *fname){
	char ext[6];
	int i=strlen(fname)-1;
	int k=0;
	do{
		i--;
	}
	while(fname[i]!='.' && i>0);
	if(i == 0) return 0;
	for(int j=i+1;j<=strlen(fname);j++){
		ext[k]=fname[j];
		k++;
	}
	//check
	if(strcmp(ext,"txt") == 0)
		return 1;
	return 0;
}

void get_parentFolPath_fname(char *path,char *par_fol_path, char *fname){
	int i= strlen(path)-1;
	int j,k=0;
	while(path[i] != '/'){
		i--;
	}	
	strncpy(par_fol_path,path,i+1);

	for(j=i+1;j<=strlen(path);j++) {
		fname[k]=path[j];
		k++;
	}
}

int main(){
	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	int bytes_sent, bytes_received;
	char path[BUFF_SIZE],//duong dan cua tep tin
	par_fol_path[BUFF_SIZE], //duong dan toi thu muc cha cua tep tin
	fname[BUFF_SIZE], //ten tep tin
	new_path[BUFF_SIZE];//duong dan cua tep tin moi chua du lieu van ban dang chu hoa
	FILE *f, *f1;
	int k, count_byte=0;
	char c;

	printf("Nhap duong dan tep tin: ");
	fgets(path,BUFF_SIZE, stdin);
	path[strlen(path)-1] = '\0';

	get_parentFolPath_fname(path, par_fol_path, fname);

	//check file format (.txt)
	if(check_file_format(fname) == 0){
		printf("Dinh dang file khong dung!\n");
		exit(0);
	}
	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit imediately! ");
		return 0;
	}
		
	//Step 4: Communicate with server	
	//--yeu cau server thay doi dinh dang cua ten file (chuyen thanh chu hoa)--
	bytes_sent= send(client_sock, fname, strlen(fname), 0);
	if (bytes_sent <= 0){
		perror("\nError: ");
		close(client_sock);
		return 0;
	}
	count_byte= count_byte + bytes_sent; //dem so byte gui toi server

	bytes_received = recv(client_sock, fname, BUFF_SIZE, 0);
	if (bytes_received <= 0){
		perror("\nError: ");
		close(client_sock);
		return 0;
	}
	fname[bytes_received] = '\0';
	strcpy(new_path, par_fol_path);
	strcat(new_path,fname);

	//--yeu cau server thay doi dinh dang van ban (chuyen thanh chu hoa)--
	if((f=fopen(path,"r")) == NULL){
		printf("Khong the mo file!\n");
		exit(0);
	}	
	f1=fopen(new_path,"w");//file moi chua du lieu da xu ly
	while(1){
		k = fgetc(f);// k la ma ASCII cua ky tu duoc doc
		if (k == EOF)
			strcpy(buff, finish);			
		else{			
			buff[0]=(char)k;
			buff[1]='\0'; //buff la chuoi chi chua 1 ky tu
		}
		//gui ky tu vua doc duoc toi server
		bytes_sent= send(client_sock, buff, 1, 0);
		if (bytes_sent <= 0){
			perror("\nError: ");			
			break;
		}
		count_byte= count_byte + bytes_sent; //dem so byte gui toi server
		
		if (strcmp(buff, finish) == 0) break;
		else{
		//nhan ky tu da xu ly
		bytes_received = recv(client_sock, buff, BUFF_SIZE, 0);
		if (bytes_received <= 0){
			perror("\nError: ");
			break;
		}
		//viet du lieu vao file moi
		c=buff[0];
		fputc(c,f1);
		}
	}
	fclose(f);
	fclose(f1);
	printf("So byte da gui toi server la: %d\n", count_byte);
	//Step 4: Close socket
	close(client_sock);
	return 0;
}
