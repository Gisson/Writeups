#include <stdio.h>

int main(){
	char input[46]="\nWelcome to this Chall!\nEnter the Key to win:\n";   //printed string is exactly 46 bytes
	gets(input);

	puts("");
	for(int i=0;i<45;i++){
		input[i]=input[i]^input[i+1];									//Xor character with the next
		printf("%hx ",input[i]);										//Print the output of the xor operation
	}
	printf("%hx",input[45]);											//Print the last bye

	return 0;
}
