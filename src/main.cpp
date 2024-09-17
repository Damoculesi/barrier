#include <iostream>
#include <argparse.h>
#include <threads.h>
#include <io.h>
#include <chrono>
#include <cstring>
#include "operators.h"
#include "helpers.h"
#include "prefix_sum.h"
#include <pthread.h>

using namespace std;

// Global barrier (can be either pthread_barrier or spin_barrier)
pthread_barrier_t pthreadBarrier;
spin_barrier* customBarrier;

int main(int argc, char **argv)
{
    // Parse args
    struct options_t opts;
    get_opts(argc, argv, &opts);

    bool sequential = false;
    if (opts.n_threads == 0) {
        opts.n_threads = 1;
        sequential = true;
    }

    //set up threads
    pthread_t *threads = sequential ? NULL : alloc_threads(opts.n_threads);

    //set up barrier
    if (opts.spin) {
        // Use custom spin barrier
        customBarrier = new spin_barrier(opts.n_threads);  // Initialize custom spin barrier
    } else {
        // Use pthread barrier
        pthread_barrier_init(&pthreadBarrier, NULL, opts.n_threads);  // Initialize pthread barrier
    }

    // Setup args & read input data
    prefix_sum_args_t *ps_args = alloc_args(opts.n_threads);
    int n_vals;
    int *input_vals, *output_vals;
    read_file(&opts, &n_vals, &input_vals, &output_vals);

    //"op" is the operator you have to use, but you can use "add" to test
    int (*scan_operator)(int, int, int);
    scan_operator = op;  // Or use add for testing purposes
    //scan_operator = add;

    fill_args(ps_args, opts.n_threads, n_vals, input_vals, output_vals,
        opts.spin, scan_operator, opts.n_loops);

    // Start timer
    auto start = std::chrono::high_resolution_clock::now();

    if (sequential)  {
        //sequential prefix scan
        output_vals[0] = input_vals[0];
        for (int i = 1; i < n_vals; ++i) {
            //y_i = y_{i-1}  <op>  x_i
            output_vals[i] = scan_operator(output_vals[i-1], input_vals[i], ps_args->n_loops);
        }
    }
    else {
        // Start threads with compute_prefix_sum as the function
        start_threads(threads, opts.n_threads, ps_args, compute_prefix_sum);

        // Wait for threads to finish
        join_threads(threads, opts.n_threads);
    }

    // End timer and print out elapsed time
    auto end = std::chrono::high_resolution_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "time: " << diff.count() << std::endl;

    // Write output data
    write_file(&opts, &(ps_args[0]));

    // Free other buffers
    free(threads);
    free(ps_args);
    // Destroy barriers
    if (opts.spin) {
        delete customBarrier;  // Destroy custom spin barrier
    } else {
        pthread_barrier_destroy(&pthreadBarrier);  // Destroy pthread barrier
    }
}
