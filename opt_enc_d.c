/**********************************************************************************
* Mark Rushmere 
* CS 344
* Project 4
* Description: This is an encoder daemon that will encrpy files sent to it.
* It listend for requests on a port specified by the user. 
**********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>

// Function Prototypes
int setSocket(int port);
int connectSocket(int socket);
char* encode(char *key, char *file);


int main(int argc, char *argv[]) {
	// If more than one argument provided, print error message
	int portNo, sockfd;
	
	if(argc != 2) {
		printf("Proper usage: opt_enc_d [listening port]\n");
		exit(1);
	}
	else {
		portNo = atoi(argv[1]);
	}
	sockfd = setSocket(portNo);
	while(1){
		// Set buffers and variables
		char command[512];
		char keyBuffer[512];
		char fileBuffer[512];
		memset(keyBuffer, '\0', 512);
		memset(fileBuffer, '\0', 512);
		memset(command, '\0', 512);
		int clientSocket;
		int n;
		char *ok = "OK";
		// Accept a client
		clientSocket = connectSocket(sockfd);
		
		// Check for error upon accept
		if(clientSocket < 0) {
			perror("connecting");
			break;
		}

		// recceive the key
		if(read(clientSocket, fileBuffer, 512) < 0) {
			perror("reading filename");
		}
		printf("the recieved command was %s\n", fileBuffer);

		// send response
		if(write(clientSocket, ok, sizeof(ok)) < 0) {
			perror("reading filename");
		}

		// Get the file

		// send ok response

		// get the key

		// send ok response

		// call function to convert the text

		// Send back the cyphertext

	}

	return 0;
}

int setSocket(int port) {
	int socketNum;
	int one = 1;
	struct sockaddr_in sa;
	socketNum = socket(AF_INET, SOCK_STREAM, 0);

	if(socketNum < 0) {
		perror("socket");
		exit(1);
	}

	// Set sockaddr fields
	memset(&sa, '\0', sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(port);

	if(setsockopt(socketNum, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) == -1) {
		perror("setsocketopt");
		exit(1);
	}
	// Check for any errors binding
	if(bind(socketNum, (struct sockaddr*) &sa, sizeof(struct sockaddr_in)) < 0) {
			printf("Error binding the socket\n");
			exit(1);
	}

	// Check for any errors listening
	if(listen(socketNum, 10) == -1) {
		perror("listen");
		exit(1);
	}
	printf("listening on port %d\n", port);
	return socketNum;
}

int connectSocket(int socket) {
	struct sockaddr_in clientAddr;
	int clientSocket;
	int clientLen = sizeof(clientAddr);
	clientSocket = accept(socket, (struct sockaddr*) &clientAddr, &clientLen);

	if(clientSocket < 0) {
		perror("accept");
		return -1;
	}
	else {
		return clientSocket;
	}
}