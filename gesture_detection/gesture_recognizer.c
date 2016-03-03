#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "gesture_trie.h"

#define GESTURE_STRING_BUFFER_SIZE 	20

static void randomizeXY(int *x, int *y, int range);
static void storeGestureFromFile(char *filename, int gesture_code);

int main(int argc, char *argv[]) {
	if ((argc < 5) || ((argc % 2) == 0)) {
		printf("./main <raw_position_data> <randomize_n> <gesture_positions> <gesture_code> ...\n");
		exit(0);
	}

	int x, y, direction_code;
	int randomize_number = atoi(argv[2]);

	// http://stackoverflow.com/questions/822323/how-to-generate-a-random-number-in-c
	srand(time(NULL));

	for (int i=3; i < argc; i+=2)
		storeGestureFromFile(argv[i], atoi(argv[i+1]));

	FILE *file = fopen(argv[1], "r");
	if (file == NULL) {
		printf("File didn't open\n");
		exit(0);
	}

	struct DirectionNode *current = getBase();
	printTrie(current);

	struct DirectionNode *incoming_node, *last_incoming_node = getBase();

	int i = -1;
	while (feof(file) == 0) {
		fscanf(file, "%d,%d\n", &x, &y);			
		randomizeXY(&x, &y, randomize_number);
		i++;
		
		incoming_node = createDirectionNode(x, y, NO_GESTURE);

		if (compareTwoDirectionNodes(incoming_node, last_incoming_node) == NODES_SAME) {
			continue;
		}

		current = nextDirectionNode(incoming_node, current, &direction_code);

		printf("%d,%d,%d,%d", i, x, y, incoming_node->grid_num);

		if (current) {
			// Leaf node
			if (current->gesture_code != NO_GESTURE) {
				printf(",#%d\n", current->gesture_code);
				current = getBase();
			} else {
				printf(",h\n");
			}
		} else {
			current = getBase();
			printf(",m\n");
		}

		last_incoming_node = incoming_node;
		usleep(100);
	}
}

static void storeGestureFromFile(char *filename, int gesture_code) {
	int gesture[GESTURE_STRING_BUFFER_SIZE][2];
	int i = 0;

	// http://stackoverflow.com/questions/3501338/c-read-file-line-by-line
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		printf("File didn't open\n");
		exit(0);
	}

	while ((i < GESTURE_STRING_BUFFER_SIZE) && (feof(file) == 0)) {
		fscanf(file, "%d,%d\n", &gesture[i][0], &gesture[i][1]);
		i++;
	}

	addGesture(gesture_code, i, gesture);
}

static void randomizeXY(int *x, int *y, int range) {
	int raw_random_x = rand() % range;
	int random_x = rand() % 2 ? -1 * raw_random_x : raw_random_x;

	int raw_random_y = rand() % range;
	int random_y = rand() % 2 ? -1 * raw_random_y : raw_random_y;

	*x += random_x;
	*y += random_y; 
}