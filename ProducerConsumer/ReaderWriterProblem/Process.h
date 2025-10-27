#ifndef PROCESS_H
#define PROCESS_H

typedef enum { READER, WRITER } ProcessType;

class Process{
    public :
        int pid;
        ProcessType type;
        int arrival_time;
        int burst_time; // idk what this is 
        int remaining_time;
        int start_time;
        int finish_time;
        bool state = true; //true represents active while false represents inactive(blocked)
        Process* next;
        Process(int pid,ProcessType type, int arrival_time, int burst_time);
        void start(int current_time);
        void finish(int current_time);
        void display() const; // idk this either
        void wakeUp();
};
#endif