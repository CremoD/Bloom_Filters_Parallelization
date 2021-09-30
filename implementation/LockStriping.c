#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include<math.h>
#include "murmur3/murmur3.h"
#include "bloom/bloom.h"
#include "dataloader/dataloader.h"
#include "utility/utility.h"


int main(int argc, char* argv[]){

	//We check if the number of parameters is the correct one
	if(argc != 7){
		fprintf(stderr, "Baseline requires 6 parameters.\nExample command: ./baseline input_file test_file parentSeed numberSeed filterSize number_threads\n");
		return 0;
	}


	//some general tests
	int (*functionPtr)(void*);
	functionPtr = &strlen;

	functionPtr = &get_size; 
	//We take the input parameters

	//take parentSeed
	int parentSeed = atoi(argv[3]);

	//take number of seeds
	int numberSeeds = atoi(argv[4]);

	//take filter size
	int filterSize = atoi(argv[5]);

	int num_threads = atoi(argv[6]);

	int chunk_size = 128; //atoi(argv[7]);
	
	//and we create a new filter with the desired specification
	BloomFilter filter = getBloomFilterLockStriping(filterSize,numberSeeds, parentSeed);
	
	D fprintf(stderr, "Filter size: %d - # hash: %d\n", filter.size, filter.hash_n);

	if(strstr(argv[1], "numeric") && strstr(argv[2], "numeric")){

		int *input, *test;
		char path[strlen(argv[1])];

		strcpy(path, argv[1]);

		input = load_int_data(argv[1]);
		test = load_int_data(argv[2]);

		int m = get_int_size(path);

		/*int nDigits = floor(log10(abs(m))) + 1;
		char dataset_name[13 + nDigits];		
		snprintf(dataset_name, 13 + nDigits, "numerical_m_%d", m);
		*/
		//experiment(filter, input, test, m, &get_size, "numerical", filterChunks);
		

	} else if(strstr(argv[1], "string") && strstr(argv[2], "string")){

		char **input, **test;

		char path[strlen(argv[1])];

		strcpy(path, argv[1]);

		input = load_char_data(argv[1]);
		test = load_char_data(argv[2]);

		int dims[2];
		get_char_size(path, dims);


		//int mDigits = floor(log10(abs(dims[0]))) + 1;
		int nDigits = floor(log10(abs(dims[1]))) + 1;
		
		char dataset_name[14 + nDigits];		
		snprintf(dataset_name, 14 + nDigits, "strings_n_%d", dims[1]);

		experimentOpenMPLockStriping(filter, input, test, dims[0], &strlen, dataset_name, num_threads, chunk_size);


	} else{
		fprintf(stderr, "Input or test file not recognised as valid.\n");
		return 0;
	}

	free(filter.bits);
	free(filter.seeds);

	return 0;
}