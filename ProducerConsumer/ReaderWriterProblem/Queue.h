#ifndef QUEUE_H
#define QUEUE_H

#include <queue>
#include "Process.h"

using namespace std; 
class Queue{
    private: 
        queue <Process *> q ;

    public :
        void enqueue(Process*p);
        Process * dequeue();
        Process *front();
        bool isEmpty() const;
        void display() const;
};
#endif