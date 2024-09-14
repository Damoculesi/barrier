#include "spin_barrier.h"
#include <iostream>

// Constructor: Initialize the spinlock, the count, and the flag
spin_barrier::spin_barrier(int num_threads) : num_threads(num_threads), count(0), flag(0) {
    // Initialize the spinlock
    pthread_spin_init(&lock, PTHREAD_PROCESS_PRIVATE);
}

// Destructor: Destroy the spinlock
spin_barrier::~spin_barrier() {
    // Destroy the spinlock
    pthread_spin_destroy(&lock);
}

// Wait function that simulates a barrier
void spin_barrier::wait() {
    pthread_spin_lock(&lock);  // Lock the spinlock

    // Increment the count of threads that have reached the barrier
    count++;

    // If this is the last thread to reach the barrier
    if (count == num_threads) {
        // Reset the count for the next use of the barrier
        count = 0;
        // Flip the flag to let all waiting threads proceed
        flag = !flag;
        pthread_spin_unlock(&lock);  // Unlock the spinlock
    } else {
        // Save the current value of the flag
        int current_flag = flag;

        // Unlock the spinlock for other threads to proceed
        pthread_spin_unlock(&lock);

        // Spin-wait until the flag changes, indicating all threads have reached the barrier
        while (flag == current_flag) {
            // Busy-wait (spinning)
            // Optional: You could add a small sleep or pause to reduce CPU usage
            asm volatile("pause" ::: "memory");  // An optional pause instruction for efficient spinning
        }
    }
}
