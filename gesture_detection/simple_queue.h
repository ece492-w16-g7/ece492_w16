#ifndef SIMPLE_QUEUE__
#define SIMPLE_QUEUE__

#include <stdlib.h>
#include <stdio.h>
#include "gesture_trie.h"

#define ARR_SIZE 	3

#define EMPTY_QUEUE -300
#define SUCCESS		400

int getXYFromQueue(int *x, int *y);
void addXYToQueue(int x, int y);
int getQueueSize(void);
void clearQueue(void);

#endif