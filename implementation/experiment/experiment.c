#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../bloom/bloom.h"
#include "experiment.h"
#include <limits.h>
#include <sys/time.h>
#include <math.h>
#include <omp.h>
#define TIMER_DEF(n) struct timeval temp_1_##n={0,0}, temp_2_##n={0,0}
#define TIMER_START(n) gettimeofday(&temp_1_##n,(struct timezone*)0)
#define TIMER_STOP(n) gettimeofday(&temp_2_##n,(struct timezone*)0)
#define TIMER_ELAPSED(n) ((temp_2_##n.tv_sec-temp_1_##n.tv_sec)*1.e6+(temp_2_##n.tv_usec-temp_1_##n.tv_usec))

void experiment(BloomFilter filter, void **input, void **test, int m, int (*sizeFunction)(void*), char dataset_name[]){
	double start, end;
	
	D fprintf(stderr, "Adding all elements to the filter...\n");

	D fprintf(stderr, "M:%d\n", m);

	//TIMER_DEF(0);

	//TIMER_START(0);
	start = omp_get_wtime(); 
	//add all input to the filter
	for(int i = 0; i < m; i++){
		setObject(filter, input[i], sizeFunction);
	}

	end = omp_get_wtime();
	//TIMER_STOP(0);

	double insert_time = (end - start)/m*1000;//TIMER_ELAPSED(0)/1.0E+6;

	D fprintf(stderr, "Testing...\n");

	//TIMER_DEF(1);
	int j = 0;
	for(int i = 0; i <m; i++){
		if (queryObject(filter, input[i], sizeFunction) == 0){
			//D fprintf(stderr, "!!!!!!!ERRRRRRRRRRORRRRRRRRRRR!!!!!!!!!!\n");
			j = j + 1;
		}
	}
	D fprintf(stderr, "Error counter:%d\n", j);
	//counting the number of false positives
	int false_pos = 0;

	start = omp_get_wtime(); 
	//TIMER_START(1);

	for(int i = 0; i < m; i++){
		false_pos += queryObject(filter, test[i], sizeFunction);
		//false_pos/(i+1) e aggiungere ad un array
	}

	end = omp_get_wtime();
	//TIMER_STOP(1);

	double query_time = (end - start)/m*1000;//TIMER_ELAPSED(1)/1.0E+6;

	int filter_bits = (filter.size*sizeof(BLOOM_FILTER_WORD)*8);

	float p = exp(((-filter.hash_n*m)/(float)filter_bits));
	
	float expected_err_rate = pow((1-p),filter.hash_n);

	D fprintf(stderr, "Dataset: %s, M: %d, Filter size: %d, Filter #k: %d, False pos: %d, False pos rate %f, Expected false pos rate: %f, Insert time: %f, Query_time: %f, Total: %f, Num_threads: %d\n",dataset_name, m, filter_bits, filter.hash_n,false_pos, ((float)false_pos/m), expected_err_rate, insert_time, query_time, insert_time + query_time, 1);
	printf("baseline,%s,%d,%d,%d,%d,%f,%f,%f,%f,%f,%d\n",dataset_name, m, filter_bits, filter.hash_n, false_pos, ((float)false_pos/m), expected_err_rate, insert_time, query_time, insert_time + query_time, 1);
}



void experimentOpenMP(BloomFilter filter, void **input, void **test, int m, int (*sizeFunction)(void*), char dataset_name[], int num_threads, int chunk_size){

	omp_set_num_threads(num_threads);
	double start, end;
	D fprintf(stderr, "Adding all elements to the filter...\n");

	D fprintf(stderr, "M:%d\n", m);

	//TIMER_DEF(0);

	//TIMER_START(0);
	start = omp_get_wtime(); 

	//add all input to the filter
	
	#pragma omp parallel for schedule(dynamic, chunk_size) shared(filter)
	for(int i = 0; i < m; i++){
		#pragma omp critical
		{
			setObject(filter, input[i], sizeFunction);
		}
	}

	end = omp_get_wtime();
	//TIMER_STOP(0);

	double insert_time = (end - start)/m*1000;//TIMER_ELAPSED(0)/1.0E+6;

	D fprintf(stderr, "Testing...\n");

	//TIMER_DEF(1);
	int j = 0;
	for(int i = 0; i <m; i++){
		if (queryObject(filter, input[i], sizeFunction) == 0){
			//D fprintf(stderr, "!!!!!!!ERRRRRRRRRRORRRRRRRRRRR!!!!!!!!!!\n");
			j = j + 1;
		}
	}
	D fprintf(stderr, "Error counter:%d\n", j);

	//counting the number of false positives
	int false_pos = 0;

	start = omp_get_wtime();
	//TIMER_START(1);

	#pragma omp parallel for reduction(+ : false_pos) shared(filter)
	for(int i = 0; i < m; i++){
		//critical region
		false_pos += queryObject(filter, test[i], sizeFunction);
	}
	

	end = omp_get_wtime();
	//TIMER_STOP(1);

	double query_time = (end - start)/m*1000;//TIMER_ELAPSED(1)/1.0E+6;

	int filter_bits = (filter.size*sizeof(BLOOM_FILTER_WORD)*8);

	float p = exp(((-filter.hash_n*m)/(float)filter_bits));
	
	float expected_err_rate = pow((1-p),filter.hash_n);

	D fprintf(stderr, "Dataset: %s, M: %d, Filter size: %d, Filter #k: %d, False pos: %d, False pos rate %f, Expected false pos rate: %f, Insert time: %f, Query_time: %f, Total: %f, Num_threads: %d, Chunk_size: %d\n",dataset_name, m, filter_bits, filter.hash_n,false_pos, ((float)false_pos/m), expected_err_rate, insert_time, query_time, insert_time + query_time, num_threads, chunk_size);
	printf("openmp,%s,%d,%d,%d,%d,%f,%f,%f,%f,%f,%d\n",dataset_name, m, filter_bits, filter.hash_n, false_pos, ((float)false_pos/m), expected_err_rate, insert_time, query_time, insert_time + query_time, num_threads);
}

void experimentOpenMPAtomic(BloomFilter filter, void **input, void **test, int m, int (*sizeFunction)(void*), char dataset_name[], int num_threads){

	omp_set_num_threads(num_threads);
	double start, end;
	D fprintf(stderr, "Adding all elements to the filter...\n");

	D fprintf(stderr, "M:%d\n", m);

	//TIMER_DEF(0);

	//TIMER_START(0);
	start = omp_get_wtime(); 

	
	//printf("Num threads: %d\n", omp_get_num_threads());
	#pragma omp parallel for schedule(static) shared(filter)
	for(int i = 0; i < m; i++){
			setObjectAtomic(filter, input[i], sizeFunction);
	}
	end = omp_get_wtime();
	//TIMER_STOP(0);

	double insert_time = (end - start)/m*1000;//TIMER_ELAPSED(0)/1.0E+6;

	D fprintf(stderr, "Testing...\n");

	//TIMER_DEF(1);
	int j = 0;
	for(int i = 0; i <m; i++){
		if (queryObject(filter, input[i], sizeFunction) == 0){
			//D fprintf(stderr, "!!!!!!!ERRRRRRRRRRORRRRRRRRRRR!!!!!!!!!!\n");
			j = j + 1;
		}
	}
	D fprintf(stderr, "Error counter:%d\n", j);

	//counting the number of false positives
	int false_pos = 0;

	start = omp_get_wtime();
	//TIMER_START(1);

	
	#pragma omp parallel for reduction(+ : false_pos)
	for(int i = 0; i < m; i++){
		//critical region
		false_pos += queryObject(filter, test[i], sizeFunction);
	}
	

	end = omp_get_wtime();
	//TIMER_STOP(1);

	double query_time = (end - start)/m*1000;//TIMER_ELAPSED(1)/1.0E+6;

	int filter_bits = (filter.size*sizeof(BLOOM_FILTER_WORD)*8);

	float p = exp(((-filter.hash_n*m)/(float)filter_bits));
	
	float expected_err_rate = pow((1-p),filter.hash_n);

	D fprintf(stderr, "Dataset: %s, M: %d, Filter size: %d, Filter #k: %d, False pos: %d, False pos rate %f, Expected false pos rate: %f, Insert time: %f, Query_time: %f, Total: %f, Num_threads: %d\n",dataset_name, m, filter_bits, filter.hash_n,false_pos, ((float)false_pos/m), expected_err_rate, insert_time, query_time, insert_time + query_time, num_threads);
	printf("atomic,%s,%d,%d,%d,%d,%f,%f,%f,%f,%f,%d\n",dataset_name, m, filter_bits, filter.hash_n, false_pos, ((float)false_pos/m), expected_err_rate, insert_time, query_time, insert_time + query_time, num_threads);
}



void experimentOpenMPReduction(BloomFilter* filter, void **input, void **test, int m, int (*sizeFunction)(void*), char dataset_name[], int num_threads, int chunk_size){

	omp_set_num_threads(num_threads);
	double start, end;
	D fprintf(stderr, "Adding all elements to the filter...\n");

	D fprintf(stderr, "M:%d\n", m);

	//TIMER_DEF(0);

	//TIMER_START(0);
	start = omp_get_wtime(); 

	static int tid = 0;
	//add all input to the filter
	
	BLOOM_FILTER_WORD *bits = malloc(filter[0].size * sizeof(BLOOM_FILTER_WORD));

	#pragma omp threadprivate(tid)
	int threads = 0;

	#pragma omp parallel 
	{
		threads = omp_get_num_threads();
		tid = omp_get_thread_num();//omp_get_num_threads();
		//printf("Num threads: %d\n", omp_get_num_threads());
	}

	#pragma omp parallel for schedule(dynamic, chunk_size) 
	for(int i = 0; i < m; i++){
		setObject(filter[tid], input[i], sizeFunction);
	}
	
	//filter.bits = malloc(sizeInit * sizeof(BLOOM_FILTER_WORD));
	//BloomFilter finalboh = getBloomFilter(filter[0].size, filter[0].hash_n, 123);
	//provare senza simd
	#pragma simd omp shared(bits) parallel for collapse(2) reduction (|:bits)
	for(int i = 0; i < filter->size; i++){
    	for (int j=0; j < num_threads; j++){
        	bits[i] |= filter[j].bits[i];
		}
	}

	end = omp_get_wtime();
	//TIMER_STOP(0);

	double insert_time = (end - start)/m*1000;//TIMER_ELAPSED(0)/1.0E+6;

	D fprintf(stderr, "Testing...\n");

	//TIMER_DEF(1);
	int j = 0;
	for(int i = 0; i <m; i++){
		if (queryObjectReduction(bits, filter[0], input[i], sizeFunction) == 0){
			//D fprintf(stderr, "!!!!!!!ERRRRRRRRRRORRRRRRRRRRR!!!!!!!!!!\n");
			j = j + 1;
		}
	}
	D fprintf(stderr, "Error counter:%d\n", j);
	//counting the number of false positives
	int false_pos = 0;

	start = omp_get_wtime();
	//TIMER_START(1);


		#pragma omp parallel for reduction(+ : false_pos) shared(filter)
		for(int i = 0; i < m; i++){
			//critical region
			false_pos += queryObjectReduction(bits, filter[0], test[i], sizeFunction);
		}

	end = omp_get_wtime();
	//TIMER_STOP(1);

	double query_time = (end - start)/m*1000;//TIMER_ELAPSED(1)/1.0E+6;

	int filter_bits = (filter->size*sizeof(BLOOM_FILTER_WORD)*8);

	float p = exp(((-filter->hash_n*m)/(float)filter_bits));
	
	float expected_err_rate = pow((1-p),filter->hash_n);

	D fprintf(stderr, "Dataset: %s, M: %d, Filter size: %d, Filter #k: %d, False pos: %d, False pos rate %f, Expected false pos rate: %f, Insert time: %f, Query_time: %f, Total: %f, Num_threads: %d, Chunk_size: %d\n",dataset_name, m, filter_bits, filter->hash_n,false_pos, ((float)false_pos/m), expected_err_rate, insert_time, query_time, insert_time + query_time, threads, chunk_size);
	printf("reduction(chunksize=%d ),%s,%d,%d,%d,%d,%f,%f,%f,%f,%f,%d\n",chunk_size, dataset_name, m, filter_bits, filter->hash_n, false_pos, ((float)false_pos/m), expected_err_rate, insert_time, query_time, insert_time + query_time, threads);
}




void experimentOpenMPLockStriping(BloomFilter filter, void **input, void **test, int m, int (*sizeFunction)(void*), char dataset_name[], int num_threads, int chunk_size){

	omp_set_num_threads(num_threads);
	double start, end;
	D fprintf(stderr, "Adding all elements to the filter...\n");

	D fprintf(stderr, "M:%d\n", m);

	//TIMER_DEF(0);

	static int tid = 0;
	//add all input to the filter
	#pragma omp threadprivate(tid)
	int threads = 0;

	#pragma omp parallel 
	{
		threads = omp_get_num_threads();
		tid = omp_get_thread_num();//omp_get_num_threads();
		//printf("Num threads: %d\n", omp_get_num_threads());
	}

	//TIMER_START(0);
	start = omp_get_wtime(); 

	
		
	#pragma omp parallel for schedule(dynamic, chunk_size) shared(filter)
	for(int i = 0; i < m; i++){
			setObjectLockStriping(filter, input[i], sizeFunction);
	}
	
	end = omp_get_wtime();
	//TIMER_STOP(0);

	double insert_time = end - start;//TIMER_ELAPSED(0)/1.0E+6;

	D fprintf(stderr, "Testing...\n");

	//TIMER_DEF(1);
	int j = 0;
	for(int i = 0; i <m; i++){
		if (queryObjectLockStriping(filter, input[i], sizeFunction) == 0){
			//D fprintf(stderr, "!!!!!!!ERRRRRRRRRRORRRRRRRRRRR!!!!!!!!!!\n");
			j = j + 1;
		}
	}
	D fprintf(stderr, "Error counter:%d\n", j);

	//counting the number of false positives
	int false_pos = 0;

	start = omp_get_wtime();
	//TIMER_START(1);


	#pragma omp parallel for reduction(+ : false_pos) shared(filter)
	for(int i = 0; i < m; i++){
		//critical region
		false_pos += queryObjectLockStriping(filter, test[i], sizeFunction);
	}
	

	end = omp_get_wtime();
	//TIMER_STOP(1);

	double query_time = end - start;//TIMER_ELAPSED(1)/1.0E+6;

	int filter_bits = (sizeof(BLOOM_FILTER_WORD)*8);

	float p = exp(((-(filter.hash_n - 1)*(m/filter.size))/(float)filter_bits));
	
	float expected_err_rate = pow((1-p),(filter.hash_n - 1));

	D fprintf(stderr, "Dataset: %s, M: %d, Filter size: %d, Filter #k: %d, False pos: %d, False pos rate %f, Expected false pos rate: %f, Insert time: %f, Query_time: %f, Total: %f, Num_threads: %d\n",dataset_name, m, filter_bits, filter.hash_n-1,false_pos, ((float)false_pos/m), expected_err_rate, insert_time, query_time, insert_time + query_time, threads);
	printf("lockStriping,%s,%d,%d,%d,%d,%f,%f,%f,%f,%f,%d\n",dataset_name, m, filter_bits, filter.hash_n-1, false_pos, ((float)false_pos/m), expected_err_rate, insert_time, query_time, insert_time + query_time, threads);
}