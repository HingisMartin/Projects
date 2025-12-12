#ifndef command_hh
#define command_hh
#include "simpleCommand.hh"

using namespace std;
struct Command {
    vector <SimpleCommand *> _simpleCommand;
    string * _outFile ;
    string * _inFile;
    string * _errFile;
    bool _background;
    bool _append ;

    Command();
    void insertSimpleCommand(SimpleCommand *_simpleCommand);
    void clear();
    void print();
    void execute();
    static SimpleCommand * _currentSimpleCommand ;

};
#endif
