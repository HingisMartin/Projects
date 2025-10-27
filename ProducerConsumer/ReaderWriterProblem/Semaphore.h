#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <pthread.h>
#include "Queue.h"
class Semaphore{
    private :
        int count = 0;
        Queue *blocked_queue ;
        pthread_mutex_t mutex; // to protect the count
        pthread_cond_t cond; // cv for wait and signal
    public :
        Semaphore(int resources);
        ~Semaphore();
        void signal();
        void wait( int id);

}
#endif