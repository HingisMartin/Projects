CC= g++
CCFLAGS = -std=c++17 -g -Wall
WARNFLAGS= -Wall -Wextra -pedantic

BISON = bison
FLEX  = flex
BISONFLAGS = -d
LEX=lex -l
YACC=yacc -y -d -t --debug


# Sources
SOURCES = Shell.cpp Command.cpp simpleCommand.cpp
OBJS = $(SOURCES:.cpp=.o)

# Bison and Flex files
YACC_FILE = shell.y
LEX_FILE  = shell.l

lex.yy.o: shell.l 
	$(LEX) -o lex.yy.cc shell.l
	$(CC) $(CCFLAGS) -c lex.yy.cc

y.tab.o: shell.y
	$(YACC) -o y.tab.cc shell.y
	$(CC) $(CCFLAGS) -c y.tab.cc

command.o: command.cc command.hh
	$(CC) $(CCFLAGS) $(WARNFLAGS) -c command.cc

simpleCommand.o: simpleCommand.cc simpleCommand.hh
	$(CC) $(CCFLAGS) $(WARNFLAGS) -c simpleCommand.cc

shell.o: shell.cc shell.hh
	$(CC) $(CCFLAGS) $(WARNFLAGS) -c shell.cc

shell: y.tab.o lex.yy.o shell.o command.o simpleCommand.o $(EDIT_MODE_OBJECTS)
		$(CC) $(CCFLAGS) $(WARNFLAGS) -o shell lex.yy.o y.tab.o shell.o command.o simpleCommand.o $(EDIT_MODE_OBJECTS)

# Clean
clean:
	rm -f $(OBJS) y.tab.o lex.yy.o y.tab.cpp y.tab.h lex.yy.c $(TARGET)

.PHONY: all clean