#include <cstdlib>
#include<cstdio>
#include<iostream>
#include <vector>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <iostream>
#include <cstring>



using namespace std ;

#include "Command.hh"
#include "Shell.hh"

extern "C" void zombie(int signal);

Command::Command(){
    _simpleCommand = vector<SimpleCommand *> ();
    _outFile = NULL;
    _inFile =  NULL ;
    _errFile = NULL ;
    _background =false;
    _append = false;
}

void Command::insertSimpleCommand(SimpleCommand * comm){
    _simpleCommand.push_back(comm);
}
void Command::print(){
    int i = 0;
    for(auto &cmd : _simpleCommand) {
        printf("  %-3d ", i++ );
        cmd->print();
    }
    printf( "\n\n" );
    printf( "  Output       Input        Error        Background  Append\n" );
    printf( "  ------------ ------------ ------------ ------------ ------------\n" );
    printf( "  %-12s %-12s %-12s %-12s %-12s\n",
            _outFile?_outFile->c_str():"default",
            _inFile?_inFile->c_str():"default",
            _errFile?_errFile->c_str():"default",
            _background?"YES":"NO",
            _append?"YES":"NO");
    printf("\n\n");

}
void zombieHandler(int signal){
    int wstatus;
    while(waitpid(-1,&wstatus,WNOHANG)!=-1);
    if(isatty(0)){
        printf("Zombie signal %d", signal);
    }
}
void Command::execute(){
    ctrl_flag = 1 ;
    struct sigaction siga;
    siga.sa_handler = zombieHandler;
    sigemptyset(&siga.sa_mask);
    siga.sa_flags = SA_RESTART;
    error(sigaction(SIGCHLD, &siga, NULL), "sigaction");

    int defaultin = dup(0);
    int defaultout = dup(1);
    int defaulterr = dup(2);
    int fdin =-1; 
    int fdout = -1;
    int fderr = -1;
    if(_simpleCommand.size() == 0){
        Shell::prompt();
        return;
    }
    if(_inFile){
        fdin = open(_inFile->c_str(),O_RDONLY);
    }
    else {
        fdin = dup(defaultin);
    }
    for (size_t i= 0 ;i<_simpleCommand.size();i++){
        // exit
    if (!strcmp(_simpleCommands[i]->_arguments[0]->c_str(), "exit")) {
        dup2(defaultin, 0);
        dup2(defaultout, 1);
        dup2(defaulterr, 2);
        close(defaultin);
        close(defaultout);
        close(defaulterr);
        close(fdin);
        // exit_print();
        printf("See ya! \n");
        exit(0);
    }
        // redirecting stdin to fdin
        dup2(fdin,0);
        close(fdin);
        if(i== _simpleCommand.size()-1){
            if(_outFile){
                if(_append){
                    fdout = open(_outFile->c_str(), O_WRONLY | O_CREAT | O_APPEND, 0666); 
                }
                else {
                    fdout = open(_outFile->c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
                }
            }
            else{
                fdout = dup(defaultout);
            }
        }
        else{
            int fdpipe[2];
            pipe(fdpipe);
            fdout = fdpipe[1];
            fdin = fdpipe[0];
        }
        dup2(fdout,1);
        close(fdout);
        vector <char *> cmd;
        for(size_t j = 0 ; j < _simpleCommand[i]->_arguments.size();j++){
            cmd.push_back(const_cast<char *>(_simpleCommand[i]->_arguments[j]->c_str()));
        }
        //built in fuctions 
        if(!strcmp(cmd[0],"setenv")){
            error(setenv(cmd[1],cmd[2],1),"setenv Failed");
            clear();
            Shell:prompt();
            return;
        }
        if(!strcmp(cmd[0],"unsetenv")){
            error(unsetenv(cmd[1]),"unsetenv");
            clear();
            Shell::prompt();
            return;
        }
        if(!strcmp(cmd[0],"cd")){
            if(_simpleCommand[i]->_arguments.size() >1){
                if(chdir(cmd[1]!=0)){
                    fprintf(stderr,"Cd can't cd to %s\n",cmd[1]);
                }
                else{
                    char *homedir = getenv("HOME");
                    error(!homedir,"getenv");
                    error(chdir(homedir),"cd");
                }
                close(defaultin);
                close(defaultout);
                close(fdin);
                clear();
                Shell::prompt();
                return;
            }
        }

        int pid = fork();
        if(pid==0){
            execvp(cmd[0],cmd.data());
        }
    }
    dup2(defaultin, 0);
    dup2(defaultout, 1);
    dup2(defaulterr, 2);
    close(defaultin);
    close(defaultout);
    close(defaulterr);
  
}
void Command::clear(){
    printf("Clearing");
    for(auto &comm : _simpleCommand) {
        delete comm;
    }
    _simpleCommand.clear();
    if(_outFile){
        delete _outFile;
    }
    _outFile = NULL;
    if(_inFile){
        delete _inFile;
    }
    _inFile = NULL;
    if(_errFile){
        delete _errFile;
    }
    _errFile = NULL;
    _background = false;
    _append = false;

}
SimpleCommand * Command::_currentSimpleCommand;
