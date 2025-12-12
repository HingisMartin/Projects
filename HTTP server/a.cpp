/******************************************************************************

Welcome to GDB Online.
  GDB online is an online compiler and debugger tool for C, C++, Python, PHP, Ruby, 
  C#, OCaml, VB, Perl, Swift, Prolog, Javascript, Pascal, COBOL, HTML, CSS, JS
  Code, Compile, Run and Debug online from anywhere in world.

*******************************************************************************/
#include <stdio.h>
#include<stdlib.h>
#include<string.h>
#include <unistd.h>
#include<iostream>
#include <sys/wait.h>

int main() { 
    int fds[2];
    for(int i = 0;i<2;i++){
    fds[i] = dup(0); 
    }
    dup2(1, fds[0]);
    dup2(fds[0], fds[1]);
    dup2(2, fds[0]);
    write(fds[1], "hello\n", 6);
}
