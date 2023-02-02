#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024

int main(int argc, char *argv[])
{
	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr;
	int bytes_received, sin_size, serv_port;
	char serv_ip[30];

	if (argc == 3)
	{
		strcpy(serv_ip, argv[1]);
		serv_port = atoi(argv[2]);
	}
	else
	{
		printf("Usage: ./client <server_ip> <server_port>\n");
	}

	// Step 1: Construct a UDP socket
	if ((client_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{ /* calls socket() */
		perror("\nError: ");
		exit(0);
	}

	// Step 2: Define the address of the server
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(serv_port);
	server_addr.sin_addr.s_addr = inet_addr(serv_ip);
	sin_size = sizeof(struct sockaddr);

	// Step 3: Communicate with server

	// send the first message to server so that server can define the address of client
	strcpy(buff, "start");
	if (sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *)&server_addr, sin_size) < 0)
	{
		perror("Error: ");
		close(client_sock);
		return 0;
	}

	while (1)
	{
		printf("\nInsert string to send:");
		memset(buff, '\0', (strlen(buff) + 1));
		fgets(buff, BUFF_SIZE, stdin);

		// stop inserting string
		if (strcmp(buff, "\n") == 0)
			break;

		// send message to server
		if (sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *)&server_addr, sin_size) < 0)
		{
			perror("Error: ");
			close(client_sock);
			return 0;
		}

		// receive message from server
		printf("Output:\n");
		for (int i = 1; i <= 2; i++)
		{
			bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0,
									  (struct sockaddr *)&server_addr, (socklen_t *)&sin_size);
			if (bytes_received < 0)
			{
				perror("Error: ");
				close(client_sock);
				return 0;
			}
			buff[bytes_received] = '\0';

			// print output
			if (strlen(buff) > 0)
				printf("%s\n", buff);
			if (strcmp(buff, "Error") == 0)
				break;
		}
	}
	close(client_sock);
	return 0;
}
