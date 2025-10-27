#include<iostream>
#include "Queue.h"

using namespace std;
void Queue::enqueue(Process* P){
    q.push(P);
}

Process * Queue::dequeue(){
    if(q.empty()) return nullptr;
    Process *frontProc = q.front();
    q.pop();
    return frontProc;
}

Process *Queue::front(){
    if(q.empty()) return nullptr;
    return q.front();
}

bool Queue::isEmpty() const{
    return q.empty();
}

void Queue::display() const{
    if(q.empty()){
        cout<<"Queue is Empty" <<endl;
    }
    cout<<"Queue content" <<endl;
    queue <Process *> tmp = q;
    while(!tmp.empty()){
        Process * proc = tmp.front();
        tmp.pop();
        proc->display();
    }
}

