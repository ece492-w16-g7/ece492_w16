#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include "gesture_trie.h"

#define HIT 						31
#define MISS 						30

#define REPEAT 						41
#define CONTINUE					40

#define GESTURE_STRING_BUFFER_SIZE 	20

static void randomizeXY(int *x, int *y, int range);
static void storeGestureFromFile(char *filename, int gesture_code, struct Threshold *thresh);

int main(int argc, char *argv[]) {
	if ((argc < 7) || ((argc % 2) == 0)) {
		printf("./main <raw_position_data> <randomize_n> <angle_thresh> <length_thresh> <gesture_positions> <gesture_code> ...\n");
		exit(0);
	}

	int old_x, old_y, x, y, direction_code, last_point_status, repeat_flag;
	int randomize_number = atoi(argv[2]);

	struct Threshold thresh;
	thresh.angle = atoi(argv[3]);
	thresh.length = atoi(argv[4]);

	// http://stackoverflow.com/questions/822323/how-to-generate-a-random-number-in-c
	srand(time(NULL));

	for (int i=5; i < argc; i+=2)
		storeGestureFromFile(argv[i], atoi(argv[i+1]), &thresh);

	FILE *file = fopen(argv[1], "r");
	if (file == NULL) {
		printf("File didn't open\n");
		exit(0);
	}

	old_x = 0;
	old_y = 0;
	last_point_status = MISS;
	repeat_flag = CONTINUE;

	struct DirectionNode *current = getBase();
	printTrie(current);

	struct DirectionNode *incoming_node;
	int i = -1;
	while (feof(file) == 0) {
		if (repeat_flag == CONTINUE) {
			fscanf(file, "%d,%d\n", &x, &y);			
			randomizeXY(&x, &y, randomize_number);
			i++;
		} else {
			repeat_flag = CONTINUE;
		}
		
		incoming_node = createDirectionNode(old_x, old_y, x, y, NO_GESTURE);
		current = nextDirectionNode(incoming_node, current, &direction_code, &thresh);

		printf("%d,%d,%d,%d,%d", i, x, y, incoming_node->angle, incoming_node->length);

		if (current) {
			// Leaf node
			if (current->gesture_code != NO_GESTURE) {
				printf(",#%d\n", current->gesture_code);
				current = getBase();
				
				last_point_status = MISS;
				old_x = 0;
				old_y = 0;
			} else {
				printf(",h\n");
				last_point_status = HIT;
				old_x = x;
				old_y = y;
			}
		} else {
			if (last_point_status == HIT) {
				last_point_status = MISS;
				old_x = 0;
				old_y = 0;
				repeat_flag = REPEAT;
			}

			current = getBase();
			printf(",m\n");
		}

		usleep(100);
	}
}

static void storeGestureFromFile(char *filename, int gesture_code, struct Threshold *thresh) {
	int gesture[GESTURE_STRING_BUFFER_SIZE][2];
	int i = 0;

	// http://stackoverflow.com/questions/3501338/c-read-file-line-by-line
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		printf("File didn't open\n");
		exit(0);
	}

	gesture[0][0] = 0;
	gesture[0][1] = 0;
	i++;

	while ((i < GESTURE_STRING_BUFFER_SIZE) && (feof(file) == 0)) {
		fscanf(file, "%d,%d\n", &gesture[i][0], &gesture[i][1]);
		i++;
	}

	addGesture(gesture_code, i, gesture, thresh);
}

static void randomizeXY(int *x, int *y, int range) {
	int raw_random_x = rand() % range;
	int random_x = rand() % 2 ? -1 * raw_random_x : raw_random_x;

	int raw_random_y = rand() % range;
	int random_y = rand() % 2 ? -1 * raw_random_y : raw_random_y;

	*x += random_x;
	*y += random_y; 
}