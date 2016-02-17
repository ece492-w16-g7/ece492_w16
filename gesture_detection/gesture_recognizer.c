#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gesture_trie.h"

static void getXY(int *x, int *y);

int main(void) {
	int x, y;

	struct DirectionNode *base = getBase();
	while (1) {
		

	}
}

static void getXY(int *x, int *y) {
	printf("x y: ");
	scanf("%d %d", x, y);	
}