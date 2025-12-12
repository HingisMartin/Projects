#ifndef simplecommand_hh
#define simplecomand_hh

#include<vector>
using namespace std;

struct SimpleCommand{
    vector <string *> _arguments;
    SimpleCommand();
    ~SimpleCommand();
    void insertArgument(string *argument);
    void print();
};
#endif


