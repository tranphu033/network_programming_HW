#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

int main(int argc, const char * argv[]) {
    //create a socket
    int network_socket;
    char buff[256];
    int send_status, bytes_count=0;

    network_socket=socket(AF_INET,SOCK_STREAM,0);
    
    //specify an address for the socket
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(5500);
    server_address.sin_addr.s_addr=inet_addr("127.0.0.1");
    
    int connection_status = connect(network_socket, (struct sockaddr *) & server_address, sizeof(server_address));
    //check for connection_status
    if(connection_status==-1){
        printf("The connection has error\n\n");
    }
    
    if(connection_status==0){
    while(1){
	    printf("Enter a message: "); //Nhap message
	    fgets(buff,sizeof(buff),stdin);
        if(strcmp(buff,"\n") == 0) continue;
        buff[strlen(buff)-1]='\0';

        //gui message cho server:
    	send_status= send(network_socket,buff, strlen(buff),0);
        if(send_status < 0){
            perror("\nError: "); break;
        }
        bytes_count += send_status; //dem so byte da gui

        if (strcmp(buff,"q")==0 || strcmp(buff,"Q")==0){ //neu xau nhap vao la "q" hoac "Q" thi ket thuc chuong trinh
            printf("Close connection!\n");
            break;
        }
        //nhan thong diep tra ve tu server:
        if(recv(network_socket, &buff, sizeof(buff), 0) <= 0){
            perror("\nError: "); break;
        }
    	//in ra thong diep:
    	printf("Reply from server: %s\n\n", buff);
    }
    //in ra so byte da gui toi server:
    printf("The number of bytes sent to server: %d\n",bytes_count);
    }
    //close the socket
    close(network_socket);
    
    return 0;
}
