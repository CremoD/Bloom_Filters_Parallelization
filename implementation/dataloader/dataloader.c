#include<stdio.h>
#include <stdint.h>
#include<stdlib.h>
#include<string.h>
#include "../utility/utility.h"

int* load_int_data(char *path){
	
	//open file	
	FILE* fp = fopen(path, "r");

	int m = get_int_size(path);	

	//allocate space
	int *array = malloc(m * sizeof(int));
	int test = 0;
	
	//read them all
	for(int i=0; i < m; i++){
		fscanf(fp, "%d", &array[i]);
	}
	
	fclose(fp);

  return array;
}

const char **load_char_data(char *path){

	//open the file
	FILE* fp = fopen(path, "r");

	int dims[2]; 
	get_char_size(path, dims);

	//printf("M: %d, N: %d\n", dims[0], dims[1]);
	
	
	char **array = malloc(dims[0] * sizeof(char*));
	
	for(int i=0; i < dims[0]; i++){

		array[i] = (char*) malloc((dims[1] + 1) * sizeof(char));
		
		fscanf(fp, "%s", array[i]);
	}

	//printf("File loaded!\n");
	
	fclose(fp);

  return array;
}
