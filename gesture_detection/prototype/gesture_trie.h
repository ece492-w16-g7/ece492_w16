#ifndef GESTURE_TRIE__
#define GESTURE_TRIE__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define NO_GESTURE	 	   	-200

#define SEQUENCE_ADDED		3000
#define INVALID_SEQUENCE   	-100

#define NODES_SAME			3010
#define NODES_DIFFERENT		3011

#define GRID_LENGTH			240
#define GRID_WIDTH			320
#define	GRID_SIZE			10
#define GRID_TOT_COL		(GRID_WIDTH / GRID_SIZE)
#define GRID_TOT_ROW		(GRID_LENGTH / GRID_SIZE)

#define GRID_NEIGHBOURS_THRESH 	2
#define GRID_DIFF_THRESH		2

#define PI 						3.14159
#define ANG_THRESH				20

#define SEARCH_MODE 			500
#define FOLLOW_MODE				501

struct DirectionNode {
	int grid_num;
    struct DirectionNode *parent;
    struct ChildDirectionNode *children;
    int gesture_code;
};

struct ChildDirectionNode {
	struct DirectionNode *direction_node;
	struct ChildDirectionNode *next;
};

struct SearchNode {
	struct DirectionNode *first;
	struct DirectionNode *second;
	struct SearchNode *next;
};

struct SearchNode *getBase(void);
struct DirectionNode *getDummyBase(void);

struct DirectionNode *firstDirectionNode(struct DirectionNode *current, struct DirectionNode *last);
struct DirectionNode *nextDirectionNode(struct DirectionNode *current, struct DirectionNode *base, struct DirectionNode *last, int *gesture_code);

int addGesture(int gesture_code, int n, int gesture_sequence[n][2]);

struct DirectionNode *createDirectionNode(int x, int y, int gesture_code);

int compareTwoDirectionNodes(struct DirectionNode *node0, struct DirectionNode *node1);

void printStorage(struct SearchNode *root);

#endif
