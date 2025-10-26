#include <stdio.h>
#include<stdlib.h>

#include "BoundedBuffer.h"

using namespace std;
template <typename T>
BoundedBuffer<T>::BoundedBuffer(int N){
	//TODO: constructor to initiliaze all the varibales declared in BoundedBuffer.h
    buffer = new T[N];
    buffer_size = N;
    buffer_count = 0;
    head = 0;
    tail=0;
    // Initialize mutex and condition variables
    pthread_mutex_init(&buffer_lock, NULL);
    pthread_cond_init(&empty, NULL);
    pthread_cond_init(&full, NULL);
}
template <typename T>
BoundedBuffer<T>::~BoundedBuffer(){
    delete[] buffer;
    pthread_mutex_destroy(&buffer_lock);
    pthread_cond_destroy(&empty);
    pthread_cond_destroy(&full);
}
template <typename T>
bool BoundedBuffer<T>::isEmpty(){
	//TODO: check is the buffer is empty
	return buffer_count==0;
}
template <typename T>
bool BoundedBuffer<T>::isFull(){ 
	//TODO: check is the buffer is empty
	return buffer_count==buffer_size;
}

template <typename T>
void BoundedBuffer<T>::append(const T& data){
	//TODO: append a data item to the circular buffer
    pthread_mutex_lock(&buffer_lock);
    while(isFull()) { 
        // when the buffer is full the producer waits
        pthread_cond_wait(&empty,&buffer_lock);
    }
    buffer[tail] = data;
    tail= (tail + 1 )%buffer_size;
    buffer_count ++ ;
    pthread_cond_signal(&full);
    pthread_mutex_unlock(&buffer_lock);
}
template <typename T>
T BoundedBuffer<T>::remove(){
	//TODO: remove and return a data item from the circular buffer
    pthread_mutex_lock(&buffer_lock);
    while(isEmpty()){
        pthread_cond_wait(&full,&buffer_lock);
    }
    T value = buffer[head];
    buffer_count--;
    head = (head+1)%buffer_size;
    pthread_cond_signal(&empty);
    pthread_mutex_unlock(&buffer_lock);
    return value;
	
}

template class BoundedBuffer<int>;
