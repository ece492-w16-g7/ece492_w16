#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "gesture_trie.h"

static void getXY(int *x, int *y);

int main(void) {
	int old_x, old_y, x, y, direction_code;

	loadPredefinedGestures();

	struct DirectionNode *current = getBase();

	printTrie(current);

	getXY(&old_x, &old_y);
	while (1) {
		getXY(&x, &y);
		
		int direction = getDirectionFromCoordinates(old_x, old_y, x, y, 5);
		current = nextDirectionNode(direction, current, &direction_code);

		if (current) {
			if (current->gesture_code != NO_GESTURE) {
				printf("Gesture #%d\n", current->gesture_code);
				current = getBase();
			}
		} else {
			current = getBase();
		}

		old_x = x;
		old_y = y;
	}
}

static void getXY(int *x, int *y) {
	printf("x y: ");
	scanf("%d %d", x, y);
}