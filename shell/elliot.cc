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
* MAY FACILITATE ACADEMIC DISHONESTY
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
#include <fcntl.h>
#include <cstring>
#include <stdlib.h>


extern "C" void zomb (int sig);
extern "C" int push_source_file(const char* filename);
void printenv();
void check_error(int ret, char* message);
void check_exit(const char* cmd_c_str);
void free_args_char_array(char** args_char_array);
extern char **environ;
void execute_subshell(char *command, char *output, size_t output_size);


Command::Command() {
   // Initialize a new vector of Simple Commands
   _simpleCommands = std::vector<SimpleCommand *>();

   _outFile = NULL;
   _inFile = NULL;
   _errFile = NULL;
   _background = false;
   _append = false;
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
   // Don't do anything if there are no simple commands
    if ( _simpleCommands.size() == 0 ) {
        Shell::prompt();
        return;
    }
    // print();
    check_exit(_simpleCommands[0]->_arguments[0]->c_str());
       

    struct sigaction sa;
    sa.sa_handler = zomb;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART; 
    int sig_zom_ret = sigaction(SIGCHLD, &sa, NULL);
    check_error(sig_zom_ret, "sigaction");

    if (strcmp(_simpleCommands[0]->_arguments[0]->c_str(), "setenv") == 0) {
        int ret;
        ret = setenv(_simpleCommands[0]->_arguments[1]->c_str() ,_simpleCommands[0]->_arguments[2]->c_str(), 1); // 1 causes the name to be made if it does not exist
        check_error(ret, "setenv failed");
        clear();
        Shell::prompt();
        return;
    }
    if (strcmp(_simpleCommands[0]->_arguments[0]->c_str(), "unsetenv") == 0) {
        int ret;
        ret = unsetenv(_simpleCommands[0]->_arguments[1]->c_str() ); // 1 causes the name to be made if it does not exist
        check_error(ret, "unsetenv failed");
        clear();
        Shell::prompt();
        return;
    }
    if (strcmp(_simpleCommands[0]->_arguments[0]->c_str(), "cd") == 0) {
       int cd_ret = -1;
       if (_simpleCommands[0]->_arguments.size() > 1) {
           if (strcmp("${HOME}", _simpleCommands[0]->_arguments[1]->c_str()) == 0) {
               cd_ret = chdir(getenv("HOME"));
           } else {
               cd_ret = chdir(_simpleCommands[0]->_arguments[1]->c_str());
           }
           if (cd_ret != 0) {
               fprintf(stderr, "cd: can't cd to %s\n", _simpleCommands[0]->_arguments[1]->c_str());
           }
       } else {
           char* home = getenv("HOME");
           if (!home) {
               fprintf(stderr, "cd: HOME not set\n");
           } else {
               cd_ret = chdir(home);
               if (cd_ret != 0) {
                   fprintf(stderr, "cd: can't cd to %s\n", home);
               }
           }
       }
       clear();
       Shell::prompt();
       return;
   }

    if (strcmp(_simpleCommands[0]->_arguments[0]->c_str(), "source") == 0) {
        if (_simpleCommands[0]->_arguments.size() < 2) {
            fprintf(stderr, "source: missing file argument\n");
            clear();
            Shell::prompt();
            return;
        }
        
        const char* filename = _simpleCommands[0]->_arguments[1]->c_str();
        
        // Use the lexer's buffer stack to handle nested source files
        if (push_source_file(filename) != 0) {
            perror("source");
            clear();
            Shell::prompt();
            return;
        }
        
        // Clear current command and return
        // The lexer will now read from the file, and when EOF is reached,
        // it will automatically return to the previous input source
        clear();
        
        // Don't call Shell::prompt() - let the shell naturally read from the file
        return;
    }


    
    // save stdin, stdout, and stderr temp files
    int tmpin = dup(0);
    int tmpout = dup(1);
    int tmperr = dup(2);
    // set initial input
    int fdin;
    int fdout;
    int fderr;
    if (_inFile) {
      // printf("input file\n");
       fdin = open(_inFile->c_str(), O_RDONLY);
    } else {
     //  printf("no file input redirection, use tmpin as file input\n");
       fdin = dup(tmpin);
    }

    int fork_ret;
    for (int i = 0; i < _simpleCommands.size(); i++) {
        int args_len = _simpleCommands[i]->_arguments.size();

        dup2(fdin, 0);
        close(fdin);
        int fdpipe[2];
        pipe(fdpipe);
        if (i == _simpleCommands.size() - 1) {
            if(_outFile){
                if (_append) {
                    fdout = open(_outFile->c_str(), O_WRONLY|O_CREAT|O_APPEND, 0666);
                } else {
                    fdout = open(_outFile->c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0666);
                }
            }
           else {   fdout=dup(tmpout);  }

            if(_errFile){
                if (_append) {
                    fderr = open(_errFile->c_str(), O_WRONLY|O_CREAT|O_APPEND, 0666);
                } else {
                    fderr = open(_errFile->c_str(), O_WRONLY|O_CREAT|O_TRUNC, 0666);
                }
            }
           else {   fderr=dup(tmperr);  }

           dup2(fderr, 2);
           close(fderr);

        } else {
            int fd_pipe[2];
            pipe(fd_pipe);
            fdin = fd_pipe[0];
            fdout = fd_pipe[1];
        }
        dup2(fdout, 1);
        close(fdout);
       
        fork_ret = fork();
        if (fork_ret == 0) {
            if (strcmp(_simpleCommands[i]->_arguments[0]->c_str(), "printenv") == 0) {
                int j = 0;
                char **env = environ;
                while (*env != NULL) {
                    printf("%s\n", *env);
                    env++;
                }
               // because its a child process 
                exit(0); 
               
            //    free_args_char_array(args_char_array);
            //    //clear();
            //    Shell::prompt();
            //    return;
           } 
           // change to C char arrays
            const char* cmd_c_str = (_simpleCommands[i]->_arguments[0])->c_str();
            //char ** args_char_array = (char**) args_to_strings(i);
            char ** args_char_array = new char*[args_len+1];
            for(int j = 0; j < args_len;j++){
                args_char_array[j] = (char*)_simpleCommands[i]->_arguments[j]->c_str();
            }
            args_char_array[args_len] =NULL;
            execvp(cmd_c_str, args_char_array);
            perror("execvp");
            exit(1);
        }
        
    }

    
    dup2(tmpin, 0);
    close(tmpin);
    dup2(tmpout, 1);
    close(tmpout);
    if (!_background) {
        waitpid(fork_ret,NULL,0);
    } 
    // else {
    //     // if(sigaction(SIGCHLD, &sa, NULL)){
    //     //     perror("sigaction");
    //     //     exit(2);
    //     // }
    // }
    clear();
    Shell::prompt();
}


SimpleCommand * Command::_currentSimpleCommand;

char** Command::args_to_strings(int cmd_num) {
    int args_len = (_simpleCommands[cmd_num]->_arguments).size();
//    printf("args_len: %d\n", args_len);
  
   // Dynamically allocate the array of char pointers
    char** args_char_array = new char*[args_len + 1];
  
    int i = 0;
    for (auto& argument : _simpleCommands[cmd_num]->_arguments) {
        args_char_array[i] = (char*) argument->c_str();
        i++;
    }
    args_char_array[i] = NULL;  // NULL terminate for execvp
    return args_char_array;
}


extern "C" void zomb( int sig ) {
    int wait_status;
    while (waitpid(-1, &wait_status,WNOHANG)!=-1);
}


void check_exit(const char* cmd_c_str) {
    if (strcmp(cmd_c_str, "exit") == 0) {
        printf("🫡\n");
        exit(0);
    }
}


void check_error(int ret, char* message) {
    if (ret != 0) {
        perror(message);
        exit(1);
    }
}

void free_args_char_array(char** args_char_array) {
    delete[] args_char_array;
//    int i;
//    for (i = 0; args_char_array[i] != nullptr; i++) {
//        delete args_char_array;
//    }
   // printf("i: %d\n", i);
   
}

/* In your command execution code */
char* expand_subshells(char *arg) {
    static char result[4096];
    char *start, *end;
    int result_pos = 0;
    char *current = arg;
    
    while (*current) {
        if (current[0] == '$' && current[1] == '(') {
            /* Found subshell start */
            start = current + 2;
            end = start;
            int depth = 1;
            
            while (*end && depth > 0) {
                if (*end == '(') depth++;
                if (*end == ')') depth--;
                if (depth > 0) end++;
            }
            
            /* Extract command */
            int cmd_len = end - start;
            char subshell_cmd[1024];
            strncpy(subshell_cmd, start, cmd_len);
            subshell_cmd[cmd_len] = '\0';
            
            /* Execute subshell */
            char output[2048];
            execute_subshell(subshell_cmd, output, sizeof(output));
            
            /* Append output to result */
            strcpy(result + result_pos, output);
            result_pos += strlen(output);
            
            current = end + 1;
        } else {
            result[result_pos++] = *current++;
        }
    }
    
    result[result_pos] = '\0';
    return result;
}

void execute_subshell(char *command, char *output, size_t output_size) {
    int pipe_to_child[2];
    int pipe_from_child[2];
    
    pipe(pipe_to_child);
    pipe(pipe_from_child);
    
    pid_t pid = fork();
    
    if (pid == 0) {
        dup2(pipe_to_child[0], 0);
        dup2(pipe_from_child[1], 1);
        close(pipe_to_child[0]);
        close(pipe_to_child[1]);
        close(pipe_from_child[0]);
        close(pipe_from_child[1]);
        
        execl("./shell", "shell", NULL);
        exit(1);
    }
    
    close(pipe_to_child[0]);
    close(pipe_from_child[1]);
    
    write(pipe_to_child[1], command, strlen(command));
    write(pipe_to_child[1], "\nexit\n", 6);
    close(pipe_to_child[1]);
    
    int nbytes = read(pipe_from_child[0], output, output_size - 1);
    close(pipe_from_child[0]);
    
    waitpid(pid, NULL, 0);
    
    if (nbytes > 0) {
        output[nbytes] = '\0';
        /* Replace newlines with spaces */
        for (int i = 0; i < nbytes; i++) {
            if (output[i] == '\n' || output[i] == '\t') {
                output[i] = ' ';
            }
        }
        /* Trim trailing space */
        if (output[nbytes - 1] == ' ') {
            output[nbytes - 1] = '\0';
        }
    } else {
        output[0] = '\0';
    }
}