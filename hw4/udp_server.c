#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#define BUFF_SIZE 1024

// split input string to letter string and number string
int splitString(char *str, char *letter_str, char *num_str)
{
	int i;
	memset(letter_str, '\0', strlen(letter_str) + 1);
	memset(num_str, '\0', strlen(num_str) + 1);

	for (i = 0; i < strlen(str); i++)
	{

		if (('a' <= str[i] && str[i] <= 'z') || ('A' <= str[i] && str[i] <= 'Z'))
		{
			strncat(letter_str, str + i, 1);
		}

		else if ('0' <= str[i] && str[i] <= '9')
		{
			strncat(num_str, str + i, 1);
		}
		else
			return 0; // if str[i] is not letter or number
	}

	letter_str[strlen(letter_str)] = '\0';
	num_str[strlen(num_str)] = '\0';
	return 1;
}

int main(int argc, char *argv[])
{
	int server_sock; /* file descriptors */
	char buff[BUFF_SIZE];
	char letters[BUFF_SIZE]; // letter string
	char nums[BUFF_SIZE];	 // number string
	int serv_port, sin_size, bytes_received;
	struct sockaddr_in server;							/* server's address information */
	struct sockaddr_in client, client1, client2, cli_t; /* client's address information */

	if (argc == 2)
	{
		serv_port = atoi(argv[1]);
	}
	else
	{
		printf("Usage: ./server <server_port>\n");
		exit(0);
	}

	// Step 1: Construct a UDP socket
	if ((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
	{ /* calls socket() */
		perror("\nError: ");
		exit(0);
	}

	// Step 2: Bind address to socket
	server.sin_family = AF_INET;
	server.sin_port = htons(serv_port); /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = INADDR_ANY;	/* INADDR_ANY puts your IP address automatically */
	bzero(&(server.sin_zero), 8);			/* zero the rest of the structure */

	if (bind(server_sock, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
	{ /* calls bind() */
		perror("\nError: ");
		exit(0);
	}

	// get client's addressed info
	memset(&client1, '\0', sizeof(client1));
	memset(&client2, '\0', sizeof(client2));
	memset(&client, '\0', sizeof(client));
	sin_size = sizeof(struct sockaddr_in);

	if (recvfrom(server_sock, buff, BUFF_SIZE, 0, (struct sockaddr *)&client1, (socklen_t *)&sin_size) < 0)
		perror("\nError: ");
	if (recvfrom(server_sock, buff, BUFF_SIZE, 0, (struct sockaddr *)&client2, (socklen_t *)&sin_size) < 0)
		perror("\nError: ");

	// Step 3: Communicate with clients
	while (1)
	{
		// wait for message from client
		bytes_received = recvfrom(server_sock, buff, BUFF_SIZE, 0, (struct sockaddr *)&client, (socklen_t *)&sin_size);
		if (bytes_received < 0)
			perror("\nError: ");
		else
		{
			buff[bytes_received - 1] = '\0';
			// print information on server
			printf("[%s:%d]: \'%s\'\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), buff);
		}

		//define the client to send message
		if (client.sin_port == client1.sin_port)
			cli_t = client2;
		else
			cli_t = client1;

		// process received string
		if (splitString(buff, letters, nums) == 0)
		{
			//send "Error" message to cli_t client
			if (sendto(server_sock, "Error", strlen("Error"), 0, (struct sockaddr *)&cli_t, sin_size) < 0)
				perror("\nError: ");
		}
		else
		{
			// send letter string and number string to cli_t client
			if (sendto(server_sock, letters, strlen(letters), 0, (struct sockaddr *)&cli_t, sin_size) < 0)
				perror("\nError: ");
			if (sendto(server_sock, nums, strlen(nums), 0, (struct sockaddr *)&cli_t, sin_size) < 0)
				perror("\nError: ");
		}
	}
	close(server_sock);
	return 0;
}
