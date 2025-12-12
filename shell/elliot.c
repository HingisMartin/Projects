/*
 * CS252: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 * DO NOT PUT THIS PROJECT IN A PUBLIC REPOSITORY LIKE GIT. IF YOU WANT 
 * TO MAKE IT PUBLICALLY AVAILABLE YOU NEED TO REMOVE ANY SKELETON CODE 
 * AND REWRITE YOUR PROJECT SO IT IMPLEMENTS FUNCTIONALITY DIFFERENT THAN
 * WHAT IS SPECIFIED IN THE HANDOUT. WE OFTEN REUSE PART OF THE PROJECTS FROM  
 * SEMESTER TO SEMESTER AND PUTTING YOUR CODE IN A PUBLIC REPOSITORY
 * MAY FACILITATE ACADEMIC DISHONESTY.
 */

#include <cstdio>
#include <cstdlib>

#include <iostream>

#include "command.hh"
#include "shell.hh"

#include <unistd.h>
#include <sys/wait.h> 

// I added
#include <stdio.h>


//char** args_to_strings(int cmd_num);
    

Command::Command() {
    // Initialize a new vector of Simple Commands
    _simpleCommands = std::vector<SimpleCommand *>();

    _outFile = NULL;
    _inFile = NULL;
    _errFile = NULL;
    _background = false;
    // _appends = false;
}

void Command::insertSimpleCommand( SimpleCommand * simpleCommand ) {
    // add the simple command to the vector
    _simpleCommands.push_back(simpleCommand);
}

void Command::clear() {
    // deallocate all the simple commands in the command vector
    for (auto simpleCommand : _simpleCommands) {
        delete simpleCommand;
    }

    // remove all references to the simple commands we've deallocated
    // (basically just sets the size to 0)
    _simpleCommands.clear();

    if ( _outFile ) {
        delete _outFile;
    }
    _outFile = NULL;

    if ( _inFile ) {
        delete _inFile;
    }
    _inFile = NULL;

    if ( _errFile ) {
        delete _errFile;
    }
    _errFile = NULL;

    _background = false;
}

void Command::print() {
    printf("\n\n");
    printf("              COMMAND TABLE                \n");
    printf("\n");
    printf("  #   Simple Commands\n");
    printf("  --- ----------------------------------------------------------\n");

    int i = 0;
    // iterate over the simple commands and print them nicely
    for ( auto & simpleCommand : _simpleCommands ) {
        printf("  %-3d ", i++ );
        simpleCommand->print();
    }

    printf( "\n\n" );
    printf( "  Output       Input        Error        Background\n" );
    printf( "  ------------ ------------ ------------ ------------\n" );
    printf( "  %-12s %-12s %-12s %-12s\n",
            _outFile?_outFile->c_str():"default",
            _inFile?_inFile->c_str():"default",
            _errFile?_errFile->c_str():"default",
            _background?"YES":"NO");
    printf( "\n\n" );
}

void Command::execute() {
    printf("in execute\n");
    fflush(stdout);
    // Don't do anything if there are no simple commands
    if ( _simpleCommands.size() == 0 ) {
        printf("no print\n");
        Shell::prompt();
        return;
    }

    // Print contents of Command data structure
    print();
    fflush(stdout);

    // Add execution here
    // For every simple command fork a new process
    // Setup i/o redirection
    // and call exec

    FILE* my_output = fopen("my_output.txt", "a");

    // save stdin, stdout, and stderr temp files
    int tmpin = dup(0);
    int tmpout = dup(1);
    int tmperr = dup(2);

    // set initial input
    int fdin;
    int fdout;
    // if there is a <
    if (_inFile) {
        // open infile
    } else {
        // use defalt input
        printf("no file input redirection, use tmpin as file input\n");
        fdin = dup(tmpin);
    }

    for (int i = 0; i < _simpleCommands.size(); i++) {
        // change to C char arrays
        char* cmd_c_str = (char*) (_simpleCommands[i]->_arguments[0])->c_str();
        printf("cmd_c_str: %s\n", cmd_c_str);
        char** args_char_array = (char**) args_to_strings(i);

        // redirect stdin to in
        dup2(fdin, 0);
        close(fdin);
    

        int fdpipe[2];
        pipe(fdpipe);

        // he is doing this for every command not just for the last command
        if(_outFile){
        // fdout=open(outfile,......);
        }
        else {
            printf("no outfile, copy tmpout to fdout\n");
            fdout=dup(tmpout);
        }

        if (i == 0 && _simpleCommands.size() > 1) {
            fprintf(my_output, "first command, redirecting output pipes\n");
            printf("on first command\n");
            dup2(fdpipe[0], fdin);
            dup2(fdout, fdpipe[1]);
        } else {
            printf("on last command\n");
            // overwrite fdpipe[0] to fdout
            dup2(fdpipe[0] ,fdout);
        }


        
        int ret = fork();
        if (ret == 0) {
            close(fdpipe[0]);
		    close(fdpipe[1]);
            printf("running: %s\n", cmd_c_str);
            execvp(cmd_c_str, args_char_array);
            //perror("execvp");
            exit(1);
        }
        dup2(fdout, 1);
        close(fdout);
        dup2(fdin,0);
        close(fdin);

        waitpid(ret,NULL,0);
    }

    dup2(tmpin, 0);
    close(tmpin);
    dup2(tmpout, 1);
    close(tmpout);


    // Clear to prepare for next command
    clear();

    // Print new prompt
    Shell::prompt();
}

SimpleCommand * Command::_currentSimpleCommand;

char** Command::args_to_strings(int cmd_num) {
    int args_len = (_simpleCommands[cmd_num]->_arguments).size();
    
    // Dynamically allocate the array of char pointers
    char** args_char_array = new char*[args_len + 1];
    
    int i = 0;
    for (auto& argument : _simpleCommands[cmd_num]->_arguments) {
        args_char_array[i] = (char*) argument->c_str();
        printf("args_char_array[%d]: %s\n", i, args_char_array[i]);
        i++;
    }
    args_char_array[i] = NULL;  // NULL terminate for execvp
    return args_char_array;
}