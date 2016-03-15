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
#include <fcntl.h>

// Max size 128 kb, plaintext4 is 76kb
#define MAX_SIZE 128000

// Function prototypes

int main(int argc, char *argv[]) {

	int portNo, sockfd, i, fp;
	struct sockaddr_in serverAddr;
	struct hostent *server;
	char *port;
	char *msgName;
	char *keyName;
	char res[4];
	char command[512];
	char keyFileLen[10];
	char msgFileLen[10];
	bzero(command, 512);
	int msgLen, keyLen;

	// Set the server to localhost
	// hardcode eos-class server because the program should not be run anywhere else
	char *serverName = "eos-class";
	// Confirmation messages
	char *ok = "ok";
	char *dec = "dec";
	char final[4];
	memset(final, '\0', 4);
	strcpy(final, ok);
	server = gethostbyname(serverName);


	if(argc > 4) {	
		printf("Proper usage: otp_dec [file] [key file] [port] \n");
		exit(1);
	}
	else {
		// Assign the arguments
		msgName = argv[1];
		keyName = argv[2];
		port = argv[3];
	}

	portNo = atoi(port);

	// Alocate memory 
	char msgFile[MAX_SIZE];
	char keyFile[MAX_SIZE];	

	// Open the file to be sent
	if((fp = open(msgName, O_RDONLY)) < 0) {
		printf("Message file not found\n");
		exit(1);
	}

	// Read file into msgFile buffer
	msgLen = read(fp, msgFile, MAX_SIZE);
	msgLen -= 1;
	// Check the file for bad characters
	for(i = 0; i < msgLen; i++) {
		// If not space or Uppercase, error
		if(((int) msgFile[i] != 32) && ((int) msgFile[i] < 65 || (int) msgFile[i] > 90)) {
			printf("Input contains bad characters\n");
			exit(1);
		}
	}	
	// Close the file
	close(fp);

	// Open the key
	if((fp = open(keyName, O_RDONLY)) < 0) {
		printf("Message file not found\n");
		exit(1);
	}

	// Read key into keyFile buffer
	keyLen = read(fp, keyFile, MAX_SIZE);
	keyLen -= 1;
	// Check the key for bad characters
	for(i = 0; i < keyLen; i++) {
		// If not space or uppercase, error
		if(((int) keyFile[i] != 32) && ((int) keyFile[i] < 65 || (int) keyFile[i] > 90)) {
			printf("Input contains bad characters\n");
			exit(1);
		}
	}	
	// Close the key
	close(fp);
	

	// Check if the key is long enough
	if(keyLen < msgLen) {
		printf("The key provided is too short\n");
		exit(0);
	}

	snprintf(msgFileLen, 10, "%d", msgLen);
	snprintf(keyFileLen, 10, "%d", keyLen);

	// Build message and send to server
	strcat(command, msgFileLen);
	strcat(command, " ");
	strcat(command, keyFileLen);
	strcat(command, " ");
	strcat(command, dec);

	//printf("the message to be sent is: %s\n", command);

	// Alocate memory 
	char decodedText[msgLen];

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


	// send the message file
	if(write(sockfd, msgFile, msgLen) < 0) {
		perror("writing to client");
		exit(2);
	}

	// check that the message was read in
	bzero(res, 4);
	if(read(sockfd, res, 4) < 0) {
		perror("reading response");
	}
	

	// Write the key file
	if(write(sockfd, keyFile, keyLen) < 0) {
		perror("writing to client");
		exit(2);
	}

	// check that the message was read in
	bzero(res, 4);
	if(read(sockfd, res, 4) < 0) {
		perror("reading response");
	}

	// ready to send the final text	
	if(write(sockfd, final, 4) < 0) {
			perror("final confirmation");
	}

	fseek(stdin,0,SEEK_END);
	// Receive decrypted text
	if(read(sockfd, decodedText, msgLen) < 0) {
		perror("reading response");
		exit(1);
	}

	fseek(stdout,0,SEEK_END);
	printf("%s\n", decodedText);
	

	close(sockfd);

	return 0;
}

