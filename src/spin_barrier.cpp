#include "spin_barrier.h"
#include <iostream>

// Constructor
spin_barrier::spin_barrier(int num_threads) : num_threads(num_threads), count(0), flag(0) {
    // Initialize the spinlock
    pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
}

// Destructor
spin_barrier::~spin_barrier() {
    pthread_spin_destroy(&lock);
}

// Wait function
void spin_barrier::wait() {
    pthread_spin_lock(&lock);

    count++;

    // If this is the last thread to reach the barrier
    if (count == num_threads) {
        // Reset the count
        count = 0;
        // Flip the flag
        flag = !flag;
        pthread_spin_unlock(&lock);
    } else {
        // Save the current value of the flag
        int current_flag = flag;

        // Unlock the spinlock for other threads to proceed
        pthread_spin_unlock(&lock);

        // Spin-wait
        while (flag == current_flag) {
            //maintain proper memory access ordering
            asm volatile("pause" ::: "memory");
        }
    }
}
