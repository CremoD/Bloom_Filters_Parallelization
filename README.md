# Bloom Filters Parallelization

This project analyses different approaches for the parallelisation of Bloom filters, space-efficient data structures used to efficiently identify the presence of an element in a set.

More specifically, we focused on CPU parallelisation, implementing various approaches using OpenMP, comparing a sequential baseline with an OpenMP baseline, an implementation using atomic operations and an algorithm based on the reduction strategy and a filter for each thread.

For more details, check the complete [report](report.pdf).
