/**********************************************************************************
* Mark Rushmere 
* CS 344
* Project 4
* Description: This is a keygen program that will returns a rendomly gererated number
* key of length specified by the user
**********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


int main(int argc, char *argv[]) {
	// If more than one argument provided, print error message
	if(argc > 2) {
		printf("Proper usage: keygen [key length]\n");
		exit(1);
	}
	else {
		srand(time(NULL));
		int keyLen = atoi(argv[1]);
		int i;
		for(i = 0; i < keyLen; i++) {	
			int randomNum = rand();
			int randomLetter= randomNum % 27;
			if(randomLetter == 0) {
				// for space
				randomLetter = 32;
			}
			else {
				// For the uppercase letters
				randomLetter += 64;
			}
			printf("%c", (char)randomLetter);
		}
		printf("\n");
	}
	return 0;
}