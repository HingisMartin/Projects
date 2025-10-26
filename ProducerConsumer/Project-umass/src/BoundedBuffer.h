#ifndef BOUNDED_BUFFER
#define BOUNDED_BUFFER

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

using namespace std;
template <typename T>
class BoundedBuffer {
    public :
        BoundedBuffer(int N);
        ~BoundedBuffer();
        void append(const T& data);
        T remove();
        bool isEmpty();
        bool isFull();
    private :
        T *buffer;
        int buffer_size;
        int buffer_count;
        int head,tail;

        pthread_mutex_t buffer_lock;
        pthread_cond_t empty;
        pthread_cond_t full;
};

#endif