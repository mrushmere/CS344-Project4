/**********************************************************************************
* Mark Rushmere 
* CS 344
* Project 4
* Description: This is an decoder daemon that will encrpy files sent to it.
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
	sockfd = setSocket(portNo);
	while(1){
		// Set buffers and variables
		char command[512];
		char buffer[512];
		memset(command, '\0', 512);
		memset(buffer, '\0', 512);
		int clientSocket;
		int n;
		char *ok = "OK";
		char conf[4];
		memset(conf, '\0', 4);
		strcpy(conf, ok);
		// Accept a client
		clientSocket = connectSocket(sockfd);
		
		// Check for error upon accept
		if(clientSocket < 0) {
			perror("connecting");
			break;
		}

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

		// Allocate memory to recieve the message and the key
		char *msgBuff = malloc(msgLen * sizeof(char));
		char *keyBuff = malloc(keyLen * sizeof(char)); 
		char *decoded = malloc(msgLen * sizeof(char));

		// send response
		if(write(clientSocket, conf, 4) < 0) {
			perror("reading file sizes");
		}

		// Get the message
		char buf[512];
		memset(buf, '\0', 512);
		if(read(clientSocket, buf, 512) < 0) {
			perror("reading message");
		}
		strcpy(msgBuff, buf);

		//printf("The message recieved is:\n");
		//printf("%s\n", msgBuff);

		// Send confirmation
		if(write(clientSocket, conf, 4) < 0) {
			perror("reading filename");
		}

		// Get the key
		memset(buf, '\0', 512);
		if(read(clientSocket, buf, 512) < 0) {
			perror("reading message");
		}
		strcpy(keyBuff, buf);
		//printf("the key received is:\n");
		//printf("%s", keyBuff);


		//printf("the first char in the key is %c", keyBuff[0]);


		// Send confirmation
		if(write(clientSocket, conf, 4) < 0) {
			perror("reading filename");
		}

		// Receive final confirmation
		bzero(conf, 4);

		if(read(clientSocket, conf, 4) < 0) {
			perror("reading message");
		}


		// call function to convert the text
		decode(keyBuff, msgBuff, decoded, msgLen);
		// Send back the decodedtext
		fseek(stdout,0,SEEK_END);
		fseek(stdin,0,SEEK_END);
		if(write(clientSocket, decoded, 512) < 0) {
			perror("reading filename");
		}

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

void decode(char* key, char* file, char* decoded, int msgLen) {

	int i;
	printf("file: %s\n", file);
	printf("key: %s\n", key);
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
		
		let1 = let1 - 64;
		let2 = let2 - 64;

		letCode = let2 - let1;
		if(letCode < 0) {
			letCode = letCode + 27;
		}
		letCode = letCode + 64;

		if(letCode == 64) {
			letCode = 32;
		}
		//printf("%d\n", letCode);
		temp3 = (char)letCode;
		decoded[i] = temp3;


	}

}