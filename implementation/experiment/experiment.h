void experiment(BloomFilter filter, void **input, void **test, int m, int (*sizeFunction)(void*), char dataset_name[]);
void experimentOpenMP(BloomFilter filter, void **input, void **test, int m, int (*sizeFunction)(void*), char dataset_name[], int num_threads, int chunk_size);
void experimentOpenMPAtomic(BloomFilter filter, void **input, void **test, int m, int (*sizeFunction)(void*), char dataset_name[], int num_threads);
void experimentOpenMPLockStriping(BloomFilter filter, void **input, void **test, int m, int (*sizeFunction)(void*), char dataset_name[], int num_threads, int chunk_size);