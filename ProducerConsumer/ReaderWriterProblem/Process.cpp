#include <iostream>
#include "Process.h"
using namespace std;
Process::Process(int pid, ProcessType type,int arrival_time,int burst_time){
    this->pid = pid;
    this->type = type;
    this->arrival_time = arrival_time;
    this->burst_time = burst_time;
    this->start_time = -1;
    this->finish_time = -1;
    this->remaining_time = burst_time;
    this->state = false;    
}


void Process::start(int current_time) {
    if(start_time==-1){
        start_time = current_time;
    }
}
void Process::finish(int current_time){
    finish_time = current_time;
}

void Process::wakeUp(){
    state = true;
}
void Process::display() const{
    cout <<"PID " << pid <<"Arrival Time " << arrival_time<< "Start Time" << start_time <<"Finish Time " << finish_time<<endl;
}