#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "murmur3/murmur3.h"
#include "bloom/bloom.h"
#include "dataloader/dataloader.h"
#include "utility/utility.h"
#include "experiment/experiment.h"


int main(int argc, char* argv[]){

	//We check if the number of parameters is the correct one
	if(argc != 7){
		fprintf(stderr, "Baseline requires 7 parameters.\nExample command: ./baseline input_file test_file parentSeed numberSeed filterSize\n");
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
	
	//take number of threads
	int num_threads = atoi(argv[6]);

	//take chunk size
	int chunk_size = 128; //atoi(argv[7]);
	
	//and we create a new filter with the desired specification
	BloomFilter* filter = getBloomFilterArray(filterSize,numberSeeds, parentSeed, num_threads);
	
	D fprintf(stderr, "Filter size: %d - # hash: %d\n", filter[0].size, filter[0].hash_n);

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
		//experimentOpenMP(filter, input, test, m, &get_size, "numerical");
		

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

		experimentOpenMPReduction(filter, input, test, dims[0], &strlen, dataset_name, num_threads, chunk_size);


	} else{
		fprintf(stderr, "Input or test file not recognised as valid.\n");
		return 0;
	}

	free(filter->bits);
	free(filter->seeds);

	return 0;
}