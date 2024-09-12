#include "prefix_sum.h"
#include "helpers.h"
#include <pthread.h>
#include <iostream>

// Global variables for barrier synchronization and shared data
extern pthread_barrier_t barrier;

// Thread function to compute local and global prefix sum
void* compute_prefix_sum(void *a)
{
    prefix_sum_args_t *args = (prefix_sum_args_t *)a;
    int n_threads = args->n_threads;
    int *input_vals = args->input_vals;
    int *output_vals = args->output_vals;
    int n_vals = args->n_vals;
    int t_id = args->t_id;  // Use t_id instead of thread_id
    int n_loops = args->n_loops;

    // Block size for each thread
    int block_size = (n_vals + n_threads - 1) / n_threads; // Ceiling division
    int start_idx = t_id * block_size;
    int end_idx = std::min(start_idx + block_size, n_vals);

    // Step 1: Compute local prefix sum sequentially within each block
    if (start_idx < end_idx) {
        output_vals[start_idx] = input_vals[start_idx];
        for (int i = start_idx + 1; i < end_idx; ++i) {
            output_vals[i] = args->op(output_vals[i - 1], input_vals[i], n_loops);
        }
    }

    // Wait for all threads to finish local computation
    pthread_barrier_wait(&barrier);

    // Step 2: Use the last element of each block to compute global prefix sums (in thread 0)
    static int *block_sums = nullptr;
    if (t_id == 0) {
        block_sums = new int[n_threads];  // Array to store last element of each block
    }

    // Store the last value of each block
    if (end_idx > start_idx) {
        pthread_barrier_wait(&barrier);  // Wait until block_sums is allocated
        block_sums[t_id] = output_vals[end_idx - 1];
    }

    // Wait for all threads to finish storing block sums
    pthread_barrier_wait(&barrier);

    if (t_id == 0) {
        // Compute prefix sum of block sums sequentially in thread 0
        for (int i = 1; i < n_threads; ++i) {
            block_sums[i] = args->op(block_sums[i - 1], block_sums[i], n_loops);
        }
    }

    // Wait for all threads to finish computing global prefix sum
    pthread_barrier_wait(&barrier);

    // Step 3: Adjust each block's prefix sum using the global block sums
    if (t_id > 0) {
        // Add the prefix sum from the previous blocks to the current block
        int adjustment = block_sums[t_id - 1];
        for (int i = start_idx; i < end_idx; ++i) {
            output_vals[i] = args->op(adjustment, output_vals[i], n_loops);
        }
    }

    // Wait for all threads to complete before returning
    pthread_barrier_wait(&barrier);

    // Thread 0 frees the block_sums array
    if (t_id == 0) {
        delete[] block_sums;
    }

    return 0;
}
