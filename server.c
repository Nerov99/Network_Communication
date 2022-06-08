#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define buff_size 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8000
#define MAX_NUM_OF_CONNECTIONS 10
#define RESPOND_WORD "hello"
#define WORD_TO_ANSWER "world"

volatile sig_atomic_t stop = 0;

void handler(int n) {
	stop = 1;
}

int main() {
	int sockfd;
	struct sockaddr_in serverAddr;

	int newSocket;
	struct sockaddr_in newAddr;
	
	struct sigaction sa;
	sa.sa_handler = handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	
	if( sigaction(SIGTSTP, &sa, NULL) == -1) {
		perror("Couldn`t set SIGTSTP handler!");
		exit(EXIT_FAILURE);
	}
	
	socklen_t addr_size;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		printf("[-]Error in connection.\n");
		exit(1);
	}
	printf("[+]Server Socket is created.\n");

	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);
	serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	if(bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
		printf("[-]Error in binding.\n");
		exit(1);
	}
	printf("[+]Bind to port %d\n", SERVER_PORT);

	if(listen(sockfd, MAX_NUM_OF_CONNECTIONS) != 0) {
		printf("[-]Error in listening.\n");
		exit(1);	
	}
	printf("[+]Listening....\n");

	char buffer[buff_size];
	bzero(buffer, sizeof(buffer));
	pid_t childpid;
	while(1) {
		if(stop == 1) {
			close(newSocket);
			exit(1);
		}
		newSocket = accept(sockfd, (struct sockaddr*)&newAddr, &addr_size);
		if(newSocket < 0) {
			exit(1);
		}
		printf("Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));

		if((childpid = fork()) == 0) {
			close(sockfd);

			while(1) {
				recv(newSocket, buffer, buff_size, 0);
				if(strcmp(buffer, ":exit") == 0) {
					printf("Disconnected from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
					break;
				} else if(strcmp(buffer, RESPOND_WORD) == 0) {
					printf("Client[%s:%d]: %s\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port), buffer);
					send(newSocket, WORD_TO_ANSWER, strlen(WORD_TO_ANSWER), 0);
					bzero(buffer, sizeof(buffer));
				} else {
					printf("Client[%s:%d]: %s\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port), buffer);
					send(newSocket, buffer, strlen(buffer), 0);
					bzero(buffer, sizeof(buffer));
				}

			}
		}

	}
	return 0;
}
