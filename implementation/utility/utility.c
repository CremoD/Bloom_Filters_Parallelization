#include<stdio.h>
#include <stdint.h>
#include<stdlib.h>
#include<string.h>

int get_int_size(char *path){

	//split filename to get final part with number of integers to read
	char *token = strtok(path,"_");
	for(int i=0; i< 2; i++){
		
        token = strtok(NULL, "_"); 
	}

	//split last part keeping only the number
	
	char *number = strtok(token,".");
	
	//convert it to an int
	int m = atoi(number);

	return m;

}

void get_char_size(char *path, int res[]){

	//split filename to get final part with number of strings to read
	char *token = strtok(path,"_");
	int n;

	for(int i=0; i< 4; i++){
        
		if(i == 2)
			n = atoi(token);

        token = strtok(NULL, "_"); 
	}

	//split last part keeping only the number
	
	char *number = strtok(token,".");
	
	int m = atoi(number);

	res[0] = m;
	res[1] = n;

	return;
}

int get_size(void *elem){
	return sizeof(elem);
}