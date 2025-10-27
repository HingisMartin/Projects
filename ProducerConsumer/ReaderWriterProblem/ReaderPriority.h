#include "Semaphore.h"
class ReaderPriority {
    private :
        Semaphore *mutex; // for protecting read count
        Semaphore *rw ; // provides read write access
        int read_count;
    public :
        ReaderPriority();
        ~ReaderPriority();
        void reader(Process *p);
        void writer(Process *p);

}