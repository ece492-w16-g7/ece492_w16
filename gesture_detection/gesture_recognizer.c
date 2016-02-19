#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "gesture_trie.h"

#define GESTURE_STRING_BUFFER_SIZE 	20

static void randomizeXY(int *x, int *y, int range);
static void storeGestureFromFile(char *filename, int gesture_code, int length_thresh, int angle_thresh);

int main(int argc, char *argv[]) {
	if ((argc < 7) || ((argc % 2) == 0)) {
		printf("./main <raw_position_data> <randomize_n> <angle_thresh> <length_thresh> <gesture_positions> <gesture_code> ...\n");
		exit(0);
	}

	int old_x, old_y, x, y, direction_code;

	int randomize_number = atoi(argv[2]);
	int angle_thresh = atoi(argv[3]);
	int length_thresh = atoi(argv[4]);

	// http://stackoverflow.com/questions/822323/how-to-generate-a-random-number-in-c
	srand(time(NULL));

	for (int i=5; i < argc; i+=2)
		storeGestureFromFile(argv[i], atoi(argv[i+1]), length_thresh, angle_thresh);

	FILE *file = fopen(argv[1], "r");
	if (file == NULL) {
		printf("File didn't open\n");
		exit(0);
	}

	old_x = 0;
	old_y = 0;

	struct DirectionNode *current = getBase();
	printTrie(current);

	int i = 0;
	while (feof(file) == 0) {
		fscanf(file, "%d,%d\n", &x, &y);
		randomizeXY(&x, &y, randomize_number);
		
		int angle = getAngleFromCoordinates(old_x, old_y, x, y, length_thresh);
		current = nextDirectionNode(angle, current, &direction_code, angle_thresh);

		printf("%d: %d,%d %d ", i, x, y, angle);

		if (current) {
			if (current->gesture_code != NO_GESTURE) {
				printf("gesture #%d ", current->gesture_code);
				current = getBase();
			}

			printf("hit\n");
		} else {
			current = getBase();
			printf("miss\n");
		}

		old_x = x;
		old_y = y;

		i++;
		usleep(100);
	}
}

static void storeGestureFromFile(char *filename, int gesture_code, int length_thresh, int angle_thresh) {
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
		gesture[i] = getAngleFromCoordinates(old_x, old_y, x, y, length_thresh);
		
		old_x = x;
		old_y = y;

		i++;
	}

	addGesture(gesture_code, gesture, i, angle_thresh);
}

static void randomizeXY(int *x, int *y, int range) {
	int raw_random_x = rand() % range;
	int random_x = rand() % 2 ? -1 * raw_random_x : raw_random_x;

	int raw_random_y = rand() % range;
	int random_y = rand() % 2 ? -1 * raw_random_y : raw_random_y;

	*x += random_x;
	*y += random_y; 
}