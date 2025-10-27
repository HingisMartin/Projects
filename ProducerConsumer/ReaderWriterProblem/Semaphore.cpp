#include "Queue.h"
#include "Process.h"
#include "Semaphore.h"

Semaphore::Semaphore(int resources){
    if (resources < 0) throw std::invalid_argument("Negative semaphore count");
    count = resources;
    //create a queue for blocked process
    blocked_queue = new Queue();
}
Semaphore::~Semaphore(){
    delete blocked_queue;
}

void Semaphore::wait(Process *p){
    --count;
    if(count < 0){
        blocked_queue->enqueue(p);
        p->state = false;
    }
    else {
        p->state = true;
    }
}

void Semaphore::signal(){
    count++;
    if(count >= 0){
        Process * p = blocked_queue->dequeue();
        if(p) p->wakeUp();
    }
}