#ifndef _SPIN_BARRIER_H
#define _SPIN_BARRIER_H

#include <pthread.h>

class spin_barrier {
public:
    spin_barrier(int num_threads);  // Constructor
    ~spin_barrier();                // Destructor
    void wait();                    // Wait method

private:
    pthread_spinlock_t lock;        // Spinlock for synchronization
    int num_threads;                // Total number of threads
    int count;                      // Tracks how many threads have arrived
    int flag;                       // Used to let threads proceed once all have arrived
};

#endif
