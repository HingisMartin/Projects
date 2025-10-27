#include "ReaderWriter.h"
#include <iostream>
using namespace std;

ReaderWriter::ReaderWriter() {
    mutex = new Semaphore(1);
    rw = new Semaphore(1);
    read_count = 0;
}

ReaderWriter::~ReaderWriter() {
    delete mutex;
    delete rw;
}

// Reader enters
void ReaderWriter::reader(Process* p) {
    // Entry section
    mutex->wait(p);          // lock read_count
    read_count++;
    if (read_count == 1) {
        rw->wait(p);         // first reader locks writers
    }
    mutex->signal();         // unlock read_count

    // Critical section
    cout << "Reader " << p->pid << " is reading..." << endl;
    p->start(0);             // simulate start time
    p->finish(1);            // simulate finish time
    p->display();

    // Exit section
    mutex->wait(p);
    read_count--;
    if (read_count == 0) {
        rw->signal();        // last reader releases writer
    }
    mutex->signal();
}

// Writer enters
void ReaderWriter::writer(Process* p) {
    rw->wait(p);              // lock exclusive access

    // Critical section
    cout << "Writer " << p->pid << " is writing..." << endl;
    p->start(0);
    p->finish(1);
    p->display();

    rw->signal();             // release exclusive access
}
