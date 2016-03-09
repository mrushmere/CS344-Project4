/**********************************************************************************
* Mark Rushmere 
* CS 344
* Project 4
* Description: This is an encoder daemon that will encrpy files sent to it.
* It listend for requests on a port specified by the user. 
**********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>


// Function prototypes

int main(int argc, char *argv[]) {

	int portNo, sockfd;
	struct sockaddr_in serverAddr;
	struct hostent *server;
	char *port;
	char *keyFile;
	char *msgFile;
	char *msgName;
	char *keyName;
	char res[4];
	char command[512];
	char keyFileLen[10];
	char msgFileLen[10];
	bzero(command, 512);
	struct stat buffer;
	size_t msgLen, keyLen;
	FILE *fp;

	// Set the server to localhost
	// hardcode eos-class server because the program should not be run anywhere else
	char *serverName = "eos-class";
	server = gethostbyname(serverName);


	if(argc > 4) {	
		printf("Proper usage: opt_enc [file] [key file] [port] \n");
		exit(1);
	}
	else {
		// Assign the arguments
		msgName = argv[1];
		keyName = argv[2];
		port = argv[3];
	}

	portNo = atoi(port);

	// Get the sizes of the message and drop the new line
	if(stat(msgName, &buffer) < 0) {
		perror("getting file");
		exit(1);
	}
	msgLen = buffer.st_size - 1;

	if(stat(keyName, &buffer) < 0) {
		perror("key file");
		exit(1);
	}
	keyLen = buffer.st_size -1;

	// Check if the key is long enough
	if(keyLen < msgLen) {
		printf("The key provided is too short\n");
		exit(0);
	}

	// Convert the message lengths to string
	printf("message legnth%d\n", msgLen);
	printf("key legnth%d\n", keyLen);

	snprintf(msgFileLen, 10, "%d", msgLen);
	snprintf(keyFileLen, 10, "%d", keyLen);

	// Build message and send to server
	strcat(command, msgFileLen);
	strcat(command, " ");
	strcat(command, keyFileLen);

	printf("the message to be sent is: %s\n", command);

	// Alocate memory 
	msgFile = (char*) malloc(msgLen * sizeof(char));
	keyFile = (char*) malloc(msgLen * sizeof(char));

	// http://linuxhowtos.org/data/6/client.c
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		perror("socket");
	}
	if(server == NULL) {
		perror("no such host");
	}

	bzero((char *) &serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serverAddr.sin_addr.s_addr, server->h_length);
	serverAddr.sin_port = htons(portNo);

	if(connect(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr))) {
		perror("connecting");
	}

	if(write(sockfd, command, 512)) {
		perror("writing command");
	}
	bzero(res, 4);

	if(read(sockfd, res, 4) < 0) {
		perror("reading response");
	}
	printf("the response is: %s\n", res);

	close(sockfd);

	return 0;
}

