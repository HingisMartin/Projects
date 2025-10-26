#include<pthread.h>
#include<stdio.h>
#include <unistd.h>
#include<fstream>
#include "BoundedBuffer.h"
#include "ProducerConsumer.h"

//TODO: add BoundedBuffer, locks and any global variables here
using namespace std;
int num_producer,num_consumer;
int p_sleep,c_sleep;
int total_items;
int produced_items=0; // keeps track of total produced item
int consumed_items=0;
BoundedBuffer<int>* sharedBuffer;
bool debug_print = false;
// to protect these shared resources we need a lock
// Function declarations
void* producer(void* threadID);
void* consumer(void* threadID);
pthread_mutex_t count_lock;
pthread_mutex_t file_lock;
ofstream outFile;
void InitProducerConsumer(int p, int c, int psleep, int csleep, int items){
	//TODO: constructor to initialize variables declared
	//also see instruction for implementation
    num_producer = p;
    num_consumer = c;
    p_sleep = psleep;
    c_sleep = csleep;
    total_items = items; 
    //pthread_mutex_init(&count_lock,NULL);
    sharedBuffer = new BoundedBuffer<int>(10);
    pthread_t* producers  = new pthread_t[p];;
    pthread_t*  consumers= new pthread_t[c];;

    outFile.open("Output.txt");
    for(long i=0 ;i<num_producer;i++){
        //create producer thread
        if(debug_print){
            cout<<"Creating Producer threads"<<endl;
        }
        pthread_create(&producers[i],NULL,producer,(void*)i);
    }
    for(long i=0 ;i<num_consumer;i++){
        //create consumer thread
        if(debug_print){
            cout<<"Creating Consumer threads" <<endl;
        }
        pthread_create(&consumers[i],NULL,consumer,(void*)i);
    }

    // wait for all threads to complete
    for(long i=0;i < p ;i++){
        pthread_join(producers[i],NULL);
    }
    for(int i=0;i<c;i++){
        pthread_join(consumers[i],NULL);
    }
    if(debug_print){
        cout<<"Destroying threads"<<endl;
    }
    outFile.close();
    pthread_mutex_destroy(&count_lock);
    pthread_mutex_destroy(&file_lock);
    delete[] producers;
    delete[] consumers;
    
}


void* producer(void* threadID){
	//TODO: producer thread, see instruction for implementation
    long id = long(threadID);
    
    while(true){
        pthread_mutex_lock(&count_lock);
        if(produced_items>=total_items){
            pthread_mutex_unlock(&count_lock);
            break;
        }
        sleep(p_sleep);
        if(debug_print){
            cout<<"Sleeping producer threads " <<id << endl;
        }
        produced_items += 1;
        if(debug_print){
            cout<<"Unlocking producer threads" << endl;
        }
        pthread_mutex_unlock(&count_lock);
    
        long item = rand() % 1000;
        sharedBuffer->append(item);
        pthread_mutex_lock(&file_lock);
        outFile << "Producer : " <<id <<" item wrote : " << item<<endl;
        pthread_mutex_unlock(&file_lock);
        if(debug_print){
            cout << "Producer : " <<id <<" item wrote : " << item<<endl;
        }
        
    }
    pthread_exit(NULL);
}

void* consumer(void* threadID){
	//TODO: consumer thread, see instruction for implementation
    long id = long(threadID);
    while(true){
        pthread_mutex_lock(&count_lock);
        if (consumed_items >= total_items && produced_items >= total_items) {
            pthread_mutex_unlock(&count_lock);
            break;
        }
        //cout<<"Sleeping consumer threads " << id << endl;
        sleep(c_sleep);
        consumed_items += 1 ;
        //cout<<"Unlocking producer threads" << endl;
        pthread_mutex_unlock(&count_lock);
        long item = sharedBuffer->remove();
        pthread_mutex_lock(&file_lock);
        outFile << "Consumer : " <<id <<" item read : " << item << endl;
        pthread_mutex_unlock(&file_lock);
        if(debug_print){
            cout << "Consumer : " <<id <<" item read : " << item << endl;
        }
        
    }
    pthread_exit(NULL);
}