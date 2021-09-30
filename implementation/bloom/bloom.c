#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "../murmur3/murmur3.h"
#include "bloom.h"
#include <limits.h>
#include <omp.h>
//#include "../xoshiro256starstar/splitmix64.h"
#include "../xoshiro256starstar/xoshiro256starstar.h"


omp_lock_t *locks;

BloomFilter getBloomFilter(int sizeInit, int k, uint64_t seed){
	
	//creating empty filter
	BloomFilter filter;

	//setting attributes
	filter.size = sizeInit;
	filter.hash_n = k;
	
	//allocating space for bits
	filter.bits = malloc(sizeInit * sizeof(BLOOM_FILTER_WORD));
	filter.seeds = malloc(k * sizeof(uint64_t));

	initialize_seeds(seed);

	for (int i = 0; i < k; i++)
	{
		filter.seeds[i] = next();
	}


	return filter;
}


BloomFilter* getBloomFilterArray(int sizeInit, int k, uint64_t seed, int num_threads){
	
	//creating empty filter
	BloomFilter * filter = malloc(num_threads * sizeof(BloomFilter));
	for(int i = 0; i < num_threads; i++)
	{
		//setting attributes
		filter[i].size = sizeInit;
		filter[i].hash_n = k;
		//allocating space for bits
		filter[i].bits = malloc(sizeInit * sizeof(BLOOM_FILTER_WORD));
		filter[i].seeds = malloc(k * sizeof(uint64_t));

		initialize_seeds(seed);

		for (int j = 0; j < k; j++)
		{
			filter[i].seeds[j] = next();
		}
	}
	return filter;
}

BloomFilter getBloomFilterLockStriping(int sizeInit, int k, uint64_t seed){
	
	//creating empty filter
	BloomFilter filter;

	//setting attributes
	filter.size = sizeInit;
	filter.hash_n = k + 1;
	
	//allocating space for bits
	filter.bits = malloc(sizeInit * sizeof(BLOOM_FILTER_WORD));
	filter.seeds = malloc((k + 1) * sizeof(uint64_t));

	initialize_seeds(seed);

	for (int i = 0; i < k + 1; i++)
	{
		filter.seeds[i] = next();
	}

	return filter;
}


void setObject(BloomFilter f, char *key, int (*sizeFunction)(void*)){
	
	//for each of our hash iteration
	int current_seed = 0, pos_word = 0, pos_bit = 0;

	for(int i=0; i<f.hash_n; i++){
		
		current_seed = f.seeds[i];

		//calculate current hash using the seed
		BLOOM_FILTER_WORD hash = murmur_hash_64(key, sizeFunction(key), current_seed);
		
		//setting corresponding bit of the filter to 1
		pos_word = hash % f.size; //meglio shift
		pos_bit = hash / 32;
		f.bits[pos_word] = f.bits[pos_word] | (1 << pos_bit); //parte pericolosa, !race condition! in condizioni normali
		//printf("Current hash %d\n", hash);
	
	}


	return;
}

void setObjectAtomic(BloomFilter f, char *key, int (*sizeFunction)(void*)){
	
	//for each of our hash iteration
	int current_seed = 0, pos_word = 0, pos_bit = 0;

	for(int i=0; i<f.hash_n; i++){
		
		current_seed = f.seeds[i];

		//calculate current hash using the seed
		BLOOM_FILTER_WORD hash = murmur_hash_64(key, sizeFunction(key), current_seed);
		
		//setting corresponding bit of the filter to 1
		pos_word = hash % f.size; //meglio shift
		pos_bit = hash / 32;
		#pragma omp atomic
		f.bits[pos_word] |= (1 << pos_bit); //parte pericolosa, !race condition! in condizioni normali
		//printf("Current hash %d\n", hash);
	
	}


	return;
}


void setObjectLockStriping(BloomFilter f, char *key, int (*sizeFunction)(void*)){
	
	//for each of our hash iteration
	int current_seed = 0, pos_word = 0, pos_bit = 0, lockID = 0;

	int first_seed = f.seeds[0];
	BLOOM_FILTER_WORD first_hash = murmur_hash_64(key, sizeFunction(key), first_seed);	
	//setting corresponding bit of the filter to 1
	int first_pos_word = first_hash % f.size; //meglio shift

	BLOOM_FILTER_WORD current_word = 0;

	for(int i=1; i<f.hash_n; i++){
		
		current_seed = f.seeds[i];

		//calculate current hash using the seed
		BLOOM_FILTER_WORD hash = murmur_hash_64(key, sizeFunction(key), current_seed);
		
		//setting corresponding bit of the filter to 1
		//pos_word = hash % f.size; //meglio shift
		pos_bit = hash / 32;
		
		//f.bits[pos_word] = f.bits[pos_word] | (1 << pos_bit); //parte pericolosa, !race condition! in condizioni normali
		current_word = current_word | (1 << pos_bit);
		//printf("Current hash %d\n", hash);
	
	}

	#pragma omp atomic
	f.bits[first_pos_word] = f.bits[first_pos_word] | current_word;

	return;
}


int queryObject(BloomFilter f, char *key, int (*sizeFunction)(void*)){

	//for each of our hash iteration

	int current_seed = 0, pos_word = 0, pos_bit = 0, mask = 0;

	for(int i=0; i<f.hash_n; i++){
		
		current_seed = f.seeds[i];

		//calculate current hash using the seed
		BLOOM_FILTER_WORD hash = murmur_hash_64(key, sizeFunction(key), current_seed);
		
		//checking if corresponding bit of the filter is 1
		pos_word = hash % f.size;
		pos_bit = hash / 32;

		mask = 1 << pos_bit;

		if((f.bits[pos_word] & mask) == 0)
			return 0; 

		
	}

	return 1;
}

int queryObjectLockStriping(BloomFilter f, char *key, int (*sizeFunction)(void*)){

	//for each of our hash iteration

	int current_seed = 0, pos_word = 0, pos_bit = 0, mask = 0;

	int first_seed = f.seeds[0];
	BLOOM_FILTER_WORD first_hash = murmur_hash_64(key, sizeFunction(key), first_seed);	
	//setting corresponding bit of the filter to 1
	int first_pos_word = first_hash % f.size; //meglio shift


	for(int i=1; i<f.hash_n; i++){
		
		current_seed = f.seeds[i];

		//calculate current hash using the seed
		BLOOM_FILTER_WORD hash = murmur_hash_64(key, sizeFunction(key), current_seed);
		
		//checking if corresponding bit of the filter is 1
		//pos_word = hash % f.size;
		pos_bit = hash / 32;

		mask = 1 << pos_bit;

		if((f.bits[first_pos_word] & mask) == 0)
			return 0; 

		
	}

	return 1;
}


int queryObjectReduction(int f [], BloomFilter filter, char *key, int (*sizeFunction)(void*)){

	//for each of our hash iteration

	int current_seed = 0, pos_word = 0, pos_bit = 0, mask = 0;

	for(int i=0; i<filter.hash_n; i++){
		
		current_seed = filter.seeds[i];

		//calculate current hash using the seed
		BLOOM_FILTER_WORD hash = murmur_hash_64(key, sizeFunction(key), current_seed);
		
		//checking if corresponding bit of the filter is 1
		pos_word = hash % filter.size;
		pos_bit = hash / 32;

		mask = 1 << pos_bit;

		if((f[pos_word] & mask) == 0)
			return 0; 

		
	}

	return 1;
}