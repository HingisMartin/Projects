#include<cstdlib>
#include<cstdio>
#include<iostream>

#include "simpleCommand.hh"

using namespace std;

SimpleCommand::SimpleCommand(){
    //create argument list [vector that stores pointers to string objects]
    _arguments = vector<string *>();

}
SimpleCommand::~SimpleCommand(){
    for(auto &arg : _arguments){
        delete arg;
    }
}

void SimpleCommand::insertArgument(string * arguments){
    _arguments.push_back(arguments);
}

void SimpleCommand::print(){
    for(auto &arg : _arguments){
        cout << "\"" << *arg << "\" \t";
    }
    cout << endl;
}