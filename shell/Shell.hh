#ifndef shell_hh
#define shell_hh

#include "Command.hh"

struct Shell {
    static void prompt();
    static Command _currentCommand;
};

#endif