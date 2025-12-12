/*
 *
 * CS-252 
 * shell.l: lexical analyzer for shell
 * You have to extend it.
 *
 */

%{

#include <cstring>
#include <cstdlib>
#include <stack>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "y.tab.hh"
#include <fcntl.h>  // for O_WRONLY

static  void yyunput (int c,char *buf_ptr  );

void myunputc(int c) {
  unput(c);
}

// Forward declaration
void process_subshell(char *command);

// Stack to manage multiple input sources for the source command
std::stack<YY_BUFFER_STATE> buffer_stack;

%}

%option noyywrap

%%



\n {
  return NEWLINE;
}

[ \t] {
  /* Discard spaces and tabs */
}

">" {
  return GREAT;
}

"|" {
  return PIPE;
}

"<" {
  return LESS;
}

">&" {
  return GREAT_AMPERSAND;
}

">>&" {
  return GREAT_GREAT_AMPERSAND;
}

"&" {
  return AMPERSAND;
}

">>" {
  return GREAT_GREAT;
}

"2>" {
  return TWO_AMPERSAND;
}


[\"][^\"]*[\"] {
  // quotes
  yylval.cpp_string = new std::string(yytext);
  int len = yylval.cpp_string->length();

  *yylval.cpp_string = yylval.cpp_string->substr(1, len - 2);
  return WORD;
}

[^ \t\n|><&]\\([^\t\n])  {
  //backslash
  /* Assume that file names have only alpha chars */
  yylval.cpp_string = new std::string(yytext);
  // Remove all backslashes
  int len = yylval.cpp_string->length();
  for (int i = 0; i < len; i++) {
    if (((*yylval.cpp_string)[i] == '\\') && ((i + 1) < len)) {
      yylval.cpp_string->erase(i, 1);
    } 
  }
  return WORD;
}

\$\{[a-zA-Z_][a-zA-Z0-9_]*\} {
  // Environment variable expansion ${VAR}
  std::string var_name = yytext;
  // Extract variable name (remove ${ and })
  var_name = var_name.substr(2, var_name.length() - 3);
  
  // Get environment variable value
  char* env_value = getenv(var_name.c_str());
  if (env_value) {
    yylval.cpp_string = new std::string(env_value);
  } else {
    yylval.cpp_string = new std::string("");
  }
  return WORD;
}

[^ \t\n><|&\"\'\\\$]+   {
  // [^ \t\n><|&\"\'\\$]+ 
  // [^ \t\n][^ \t\n]*
  // regular words
  /* Assume that file names have only alpha chars */
  yylval.cpp_string = new std::string(yytext);
  return WORD;
}


<<EOF>> {
    // Handle end of file - pop buffer stack if we're in a sourced file
    if (!buffer_stack.empty()) {
        yy_delete_buffer(YY_CURRENT_BUFFER);
        yy_switch_to_buffer(buffer_stack.top());
        buffer_stack.pop();
    } else {
        yyterminate();
    }
}

\$\([^\n]\)|\'[^\n\']\' {
 std::string text = std::string(yytext);
  if (text[0] == '$') {
    text = text.substr(2, text.length() - 3);
    if (text.length() == 1) {
      if (text[0] == '$') {
        printf("pid: %s\n", getpid);
        text = getpid();
        std::cout << text << "\n";
      // } if else () {

      // } if else () {
        
      // } if else () {
        
      // } if else () {
  
      }
    }
  } else {
    text = text.substr(1, text.length() - 2);
  }
  text.append("\nexit\n");
  
  int shell_to_sub[2];
  int sub_to_shell[2];
  
  if (pipe(shell_to_sub) < 0) {
    perror("pipe shell_to_sub failed");
    exit(1);
  }
  if (pipe(sub_to_shell) < 0) {
    perror("pipe sub_to_shell failed");
    exit(1);
  }
  
  int fork_ret = fork();
  
  if (fork_ret == 0) {
    dup2(shell_to_sub[0], 0);
    dup2(sub_to_shell[1], 1);
    
    close(shell_to_sub[1]);
    close(sub_to_shell[0]);
    close(shell_to_sub[0]);
    close(sub_to_shell[1]);
    
    execlp("/proc/self/exe", "shell", NULL);
    perror("execlp failed");
    exit(1);
    
  } else if (fork_ret < 0) {
    perror("fork failed");
    exit(1);
  }
  
  close(shell_to_sub[0]);
  close(sub_to_shell[1]);
  
  write(shell_to_sub[1], text.c_str(), text.length());
  close(shell_to_sub[1]);
  
  waitpid(fork_ret, NULL, 0);
  
  char c;
  std::string out_subshell = "";
  
  while(read(sub_to_shell[0], &c, 1) > 0) {
    if (c == '\n') {
      out_subshell.push_back(' ');
    } else {
      out_subshell.push_back(c);
    }
  }
  close(sub_to_shell[0]);
  
  for (int i = out_subshell.length() - 1; i >= 0; i--) {
    myunputc(out_subshell[i]);
  }
}
%%


// Function to start reading from a file (for source command)
extern "C" int push_source_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        return -1;  // Error opening file
    }
    
    // Push current buffer onto stack
    buffer_stack.push(YY_CURRENT_BUFFER);
    
    // Switch to new buffer for the file
    yy_switch_to_buffer(yy_create_buffer(file, YY_BUF_SIZE));
    
    return 0;  // Success
}