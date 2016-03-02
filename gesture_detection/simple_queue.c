#include "simple_queue.h"

static int size = 0;

static int start = 0;
static int end = 0;

static int array[ARR_SIZE][2];

int getXYFromQueue(int *x, int *y) {
	if (size == 0) 
		return EMPTY_QUEUE;

	*x = array[start][0];
	*y = array[start][1];
	start = (start + 1 + ARR_SIZE) % ARR_SIZE;
	size--;

	return SUCCESS;
}

void addXYToQueue(int x, int y) {
	if (size == ARR_SIZE) {
		int m,n;
		getXYFromQueue(&m, &n);
	}

	array[end][0] = x;
	array[end][1] = y;
	end = (end + 1 + ARR_SIZE) % ARR_SIZE;
	size++;	
}

int getQueueSize(void) {
	return size;
}

void clearQueue(void) {
	size = 0;
	start = 0;
	end = 0;
}