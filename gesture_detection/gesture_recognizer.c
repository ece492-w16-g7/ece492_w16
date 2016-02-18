#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "gesture_trie.h"

#define GESTURE_STRING_BUFFER_SIZE 	20

static void randomizeXY(int *x, int *y, int range);
static void storeGestureFromFile(char *filename, int gesture_code, int direction_thresh);

int main(int argc, char *argv[]) {
	if ((argc < 6) || (argc % 2)) {
		printf("./main <raw_position_data> <randomize_n> <thresh> <gesture_positions> <gesture_code> ...\n");
		exit(0);
	}

	int old_x, old_y, x, y, direction_code;

	int randomize_number = atoi(argv[2]);
	int direction_thresh = atoi(argv[3]);

	// http://stackoverflow.com/questions/822323/how-to-generate-a-random-number-in-c
	srand(time(NULL));

	for (int i=4; i < argc; i+=2)
		storeGestureFromFile(argv[i], atoi(argv[i+1]), direction_thresh);

	FILE *file = fopen(argv[1], "r");
	if (file == NULL) {
		printf("File didn't open\n");
		exit(0);
	}

	fscanf(file, "%d,%d\n", &old_x, &old_y);
	randomizeXY(&old_x, &old_y, randomize_number);

	struct DirectionNode *current = getBase();
	printTrie(current);

	int i = 0;
	while (feof(file) == 0) {
		fscanf(file, "%d,%d\n", &x, &y);
		randomizeXY(&x, &y, randomize_number);
		
		int direction = getDirectionFromCoordinates(old_x, old_y, x, y, direction_thresh);
		current = nextDirectionNode(direction, current, &direction_code);

		printf("%d: %d,%d %d ", i, x, y, direction);

		if (current) {
			if (current->gesture_code != NO_GESTURE) {
				printf("gesture #%d ", current->gesture_code);
				current = getBase();
			}

			printf("hit: %d\n", i);
		} else {
			current = getBase();
			printf("miss: %d\n", i);
		}

		old_x = x;
		old_y = y;

		i++;
		usleep(100);
	}
}

static void storeGestureFromFile(char *filename, int gesture_code, int direction_thresh) {
	int gesture[GESTURE_STRING_BUFFER_SIZE];
	int i = 0, old_x = 0, old_y = 0, x = 0, y = 0;

	// http://stackoverflow.com/questions/3501338/c-read-file-line-by-line
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		printf("File didn't open\n");
		exit(0);
	}

	fscanf(file, "%d,%d\n", &old_x, &old_y);

	while ((i < GESTURE_STRING_BUFFER_SIZE) && (feof(file) == 0)) {
		fscanf(file, "%d,%d\n", &x, &y);
		gesture[i] = getDirectionFromCoordinates(old_x, old_y, x, y, direction_thresh);
		
		old_x = x;
		old_y = y;

		i++;
	}

	addGesture(gesture_code, gesture, i);
}

static void randomizeXY(int *x, int *y, int range) {
	int raw_random_x = rand() % range;
	int random_x = rand() % 2 ? -1 * raw_random_x : raw_random_x;

	int raw_random_y = rand() % range;
	int random_y = rand() % 2 ? -1 * raw_random_y : raw_random_y;

	*x += random_x;
	*y += random_y; 
}