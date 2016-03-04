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

struct DirectionNode {
	int grid_num;
    struct DirectionNode *parent;
    struct ChildNode *children;
    int gesture_code;
};

struct ChildNode {
	struct DirectionNode *direction_node;
	struct ChildNode *next;
};

/**
 * Returns base of tree. Note that this stores a static variable.
 * @return  Returns pointer to base of tree.
 */
struct DirectionNode *getBase(void);

/**
 * Finds the next node to move to given an angle and the current direction.
 * @param  next        	The searching node.
 * @param  current      The current node.
 * @param  gesture_code Pointer to an int for returning gesture_code. Set to gesture_code
 *                      only if a leaf node is found. Otherwise, set to NO_GESTURE.
 * @param  thresh 		The angle and length threshold used to compare angles.
 * @return              Returns NULL if DNE. Else, returns child with correct angle. Will
 *                      set gesture_code if leaf node.
 */
struct DirectionNode *nextDirectionNode(struct DirectionNode *next, struct DirectionNode *current, int *gesture_code);

/**
 * Adds a gesture sequence to the tree.
 * @param  gesture_code     The gesture code for the given gesture.
 * @param  gesture_sequence An int array with x,y coordinates.
 * @param  n                The number of points in the gesture.
 * @param  thresh 			The angle and length threshold used to compare angles.
 * @return                  Returns 0 if successful and -1 if error.
 */
int addGesture(int gesture_code, int n, int gesture_sequence[n][2]);

/**
 * @param  x0
 * @param  y0
 * @param  x1
 * @param  y1
 * @param  gesture_code gesture_code should be NO_GESTURE if not a leaf DirectionNode.
 * @return              Returns created DirectionNode.
 */ 
struct DirectionNode *createDirectionNode(int x, int y, int gesture_code);
	
int compareTwoDirectionNodes(struct DirectionNode *node0, struct DirectionNode *node1);

void printTrie(struct DirectionNode *root);
void printNode(struct DirectionNode *node);

#endif