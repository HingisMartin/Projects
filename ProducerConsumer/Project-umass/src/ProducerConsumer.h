#ifndef ProducerConsumer
#define ProducerConsumer

#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<fstream>
#include "BoundedBuffer.h"

using namespace std;

void InitProducerConsumer(int p, int c, int psleep, int csleep, int items);
void* Producer(void*);
void* Consumer(void*);

#endif