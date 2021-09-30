#include <stdio.h>
#include <stdint.h>

#define BLOOM_FILTER_WORD uint32_t

/*#ifdef DEBUG
#define LOG(message, ...) printf(message, ...)
#else
#define LOG(message, ...)
#endif
*/
#ifdef DEBUG
 #define D if(1) 
#else
 #define D if(0) 
#endif


typedef struct BloomStruct{
	
	int hash_n; 
	int size;
	 
	BLOOM_FILTER_WORD *bits;
	uint64_t *seeds;

} BloomFilter;

BloomFilter getBloomFilter(int size, int k, uint64_t seed);
BloomFilter* getBloomFilterArray(int sizeInit, int k, uint64_t seed, int num_threads);

BloomFilter getBloomFilterLockStriping(int size, int k, uint64_t seed);


void setObject(BloomFilter f, char *key, int (*sizeFunction)(void*));
void setObjectLockStriping(BloomFilter f, char *key, int (*sizeFunction)(void*));

int queryObject(BloomFilter f, char *key, int (*sizeFunction)(void*));
int queryObjectReduction(int f [], BloomFilter filter, char *key, int (*sizeFunction)(void*));