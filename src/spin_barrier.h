#ifndef _SPIN_BARRIER_H
#define _SPIN_BARRIER_H

#include <pthread.h>

class spin_barrier {
public:
    spin_barrier(int num_threads);
    ~spin_barrier();
    void wait();

private:
    pthread_spinlock_t lock;
    int num_threads;
    int count;
    int flag;
};

#endif
