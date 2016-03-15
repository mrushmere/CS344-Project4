/**********************************************************************************
* Mark Rushmere 
* CS 344
* Project 4
* Description: This is an decoder daemon that will decrypt files sent to it.
* It listens for requests on a port specified by the user and accepts requests from 
* otp_dec. It recieved the cypher text and key from otp_dec and sends back the 
* unencrypted message.
**********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>

// Function Prototypes
int setSocket(int port);
int connectSocket(int socket);
void decode(char *key, char *file, char* decoded, int msgLen);



int main(int argc, char *argv[]) {
	// If more than one argument provided, print error message
	int portNo, sockfd;
	int keyLen, msgLen;
	

	if(argc != 2) {
		printf("Proper usage: opt_enc_d [listening port]\n");
		exit(1);
	}
	else {
		portNo = atoi(argv[1]);
	}

	if(portNo < 0 || portNo > 65500) {
		printf("port number must be between 0 and 65500\n");
		exit(1);
	}


	sockfd = setSocket(portNo);



	while(1){

		// Set buffers and variables
		char buffer[512];
		int clientSocket;
		int n, pid;
		char *ok = "OK";
		char *no = "NO";
		char conf[4];
		char comType[4];
		memset(conf, '\0', 4);
		memset(comType, '\0', 4);
		strcpy(conf, ok);

		
		// Accept a client
		clientSocket = connectSocket(sockfd);
		
		// Check for error upon accept
		if(clientSocket < 0) {
			perror("connecting");
			break;
		}

		// New process
		pid = fork();
		if(pid < 0) {
			perror("forking");
		}
		if(pid == 0 ) {
			// recceive the command
			if(read(clientSocket, buffer, 512) < 0) {
				perror("reading filename");
			}
			//printf("the recieved command was %s\n", buffer);

			// Parse the command
			char *temp;
			temp = strtok(buffer, " \n\0");
			msgLen = atoi(temp);
			temp = strtok(NULL, " \n\0");
			keyLen = atoi(temp);
			temp = strtok(NULL, " \n\0");
			strcpy(comType, temp);

			// If the request did not come from opt_enc, send back error
			if(strcmp(comType, "dec") != 0) {
				memset(conf, '\0', 4);
				strcpy(conf, no);
				if(write(clientSocket, conf, 4) < 0) {
					perror("sending wrong type msg\n");
				}
				close(clientSocket);
			}

			// Allocate memory to recieve the message and the key
			char *msgBuff = malloc(msgLen * sizeof(char));
			char *keyBuff = malloc(keyLen * sizeof(char)); 
			char *decoded = malloc(msgLen * sizeof(char));

			// send response
			if(write(clientSocket, conf, 4) < 0) {
				perror("reading file sizes");
			}

			// Get the message
			if(read(clientSocket, msgBuff, msgLen) < 0) {
				perror("reading message");
			}

			// Send confirmation
			if(write(clientSocket, conf, 4) < 0) {
				perror("reading filename");
			}

			// Get the key
			if(read(clientSocket, keyBuff, keyLen) < 0) {
				perror("reading message");
			}

			// Send confirmation
			if(write(clientSocket, conf, 4) < 0) {
				perror("reading filename");
			}

			// Receive final confirmation
			bzero(conf, 4);

			if(read(clientSocket, conf, 4) < 0) {
				perror("reading message");
			}


			// call function to decode the text
			decode(keyBuff, msgBuff, decoded, msgLen);
			// Send back the decodedtext
			fseek(stdout,0,SEEK_END);
			fseek(stdin,0,SEEK_END);
			if(write(clientSocket, decoded, msgLen) < 0) {
				perror("reading filename");
			}

			free(msgBuff);
			free(keyBuff);
			free(decoded);


			close(clientSocket);
			close(sockfd);
			exit(0);
		}
		else {
			close(clientSocket);
		}
	}

	return 0;
}

// Function to set a socket up on a port, and listen for requests
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
	fprintf(stderr, "listening on port %d\n", port);
	return socketNum;
}

// Function to handle accepting client connections
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

void decode(char* key, char* file, char* decoded, int msgLen) {

	int i;
	//printf("file: %s\n", file);
	//printf("key: %s\n", key);
	for(i = 0; i < msgLen; i++) {
		char temp1, temp2, temp3;
		int let1, let2, letCode;

		temp1 = key[i];
		let1 = (int)temp1;
		temp2 = file[i];
		let2 = (int)temp2;
		// transform spaces
		if(file[i] == ' ') {
			let2 = 64;
		}
		if(key[i] == ' ') {
			let1 = 64;
		}
		
		// Make numbers in range 0 - 26
		let1 = let1 - 64;
		let2 = let2 - 64;

		letCode = let2 - let1;
		// If negative, add 27 becuase of the mod 27 for encryption 
		if(letCode < 0) {
			letCode = letCode + 27;
		}

		// Make everything uppercase
		letCode = letCode + 64;

		// Transform spaces again
		if(letCode == 64) {
			letCode = 32;
		}

		temp3 = (char)letCode;
		decoded[i] = temp3;
	}

}