#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8000
#define EXIT_COMMAND ":exit"

int main() {

	int clientSocket;
	struct sockaddr_in serverAddr;

	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSocket < 0) {
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Client Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	if(connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Connected to Server.\n");

	size_t buff_size = 1024;
	char buffer[buff_size];
	char *b = buffer;
	
	while(1) {
		printf("Client message: \t");
		getline(&b, &buff_size, stdin);
		buffer[strcspn(buffer, "\n")] = 0;
		send(clientSocket, buffer, strlen(buffer), 0);

		if(strcmp(buffer, EXIT_COMMAND) == 0) {
			close(clientSocket);
			printf("[-]Disconnected from server.\n");
			exit(1);
		}

		if(recv(clientSocket, buffer, buff_size, 0) < 0) {
			printf("[-]Error in receiving data.\n");
		} else {
			printf("Server message: \t%s\n\n", buffer);
		}
	}
	return 0;
}
