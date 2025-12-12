


/*
 * CS-252
 * shell.y: parser for shell
 *
 * This parser compiles the following grammar:
 *
 *	cmd [arg]* [> filename]
 *
 * you must extend it to understand the complete shell grammar
 *
 */
%code requires
{
#include <string>

#if __cplusplus > 199711L
#define register      // Deprecated in C++11 so remove the keyword
#endif
}

%union
{
  char        *string_val;
  // Example of using a c++ type in yacc
  std::string *cpp_string;
}

%token <cpp_string> WORD
%token NOTOKEN GREAT NEWLINE GREATGREAT LESS AND TWO GAND GGAND PIPE
%token GREATGREATAMPERSAND AMPERSAND TWOGREAT GREATAMPERSAND
%{
  //#define yylex yylex
#include <cstdio>
#include "shell.hh"

  void yyerror(const char * s);
  int yylex();

  %}

  %%

  goal:
  command_list
  ;

command_list:
commands
| command_list commands
;

commands:
pipe_list iomodifier_list background NEWLINE {
  //printf("   Yacc: Execute command\n");
  Shell::_currentCommand.execute();
}
| NEWLINE
| error NEWLINE { yyerrok; }
;


pipe_list:
simple_command
| pipe_list PIPE simple_command
;


simple_command:
command_and_args
;

command_and_args:
command_word argument_list {
  Shell::_currentCommand.
    insertSimpleCommand( Command::_currentSimpleCommand );
}
;

argument_list:
argument_list argument
| /* can be empty */
;

argument:
WORD {
  //printf("   Yacc: insert argument \"%s\"\n", $1->c_str());
  Command::_currentSimpleCommand->insertArgument( $1 );\
}
;

command_word:
WORD {
  //printf("   Yacc: insert command \"%s\"\n", $1->c_str());
  Command::_currentSimpleCommand = new SimpleCommand();
  Command::_currentSimpleCommand->insertArgument( $1 );
}
;

iomodifier_list:
iomodifier_list iomodifier_opt
| iomodifier_opt
| /* can be empty */
;

iomodifier_opt:
GREAT WORD {
  if (Shell::_currentCommand._outFile) {
    printf("Ambiguous output redirect.\n");
    exit(1);
  }
  //printf("   Yacc: insert output \"%s\"\n", $2->c_str());
  Shell::_currentCommand._outFile = $2;
}
| GREATGREAT WORD {
  if (Shell::_currentCommand._outFile) {
    printf("Ambiguous output redirect.\n");
    exit(1);
  }
  //printf("   Yacc: insert append output \"%s\"\n", $2->c_str());
  Shell::_currentCommand._outFile = $2;
  Shell::_currentCommand._append = true;
}
| LESS WORD {
  if (Shell::_currentCommand._inFile) {
    printf("Ambiguous input redirect.");
    exit(1);
  }
  //printf("   Yacc: insert input \"%s\"\n", $2->c_str());
  Shell::_currentCommand._inFile = $2;
}
| GAND WORD {
  if (Shell::_currentCommand._outFile || Shell::_currentCommand._errFile) {
    printf("Ambiguous output redirect.\n");
    exit(1);
  }
 // printf("   Yacc: insert output gand \"%s\"\n", $2->c_str());
  Shell::_currentCommand._outFile = $2;
  Shell::_currentCommand._errFile = $2;
}
| GGAND WORD {
  if (Shell::_currentCommand._outFile || Shell::_currentCommand._errFile) {
    printf("Ambiguous output redirect.\n");
    exit(1);
  }
  //printf("   Yacc: insert output ggand \"%s\"\n", $2->c_str());
  Shell::_currentCommand._outFile = $2;
  Shell::_currentCommand._errFile = $2;
  Shell::_currentCommand._append = true;
}
| TWO WORD {
  if (Shell::_currentCommand._errFile) {
    printf("Ambiguous output redirect.\n");
    exit(1);
  }
  //printf("   Yacc: insert error \"%s\"\n", $2->c_str());
  Shell::_currentCommand._errFile = $2;
};

background:
AND {
  //printf("   Yacc: set background\n");
  Shell::_currentCommand._background = true;
}
| /* can be empty */
;
%%

  void
yyerror(const char * s)
{
  fprintf(stderr,"%s", s);
}

#if 0
main()
{
  yyparse();
}
#endif



