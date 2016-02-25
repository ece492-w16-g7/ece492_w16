#ifndef GESTURE_TRIE__
#define GESTURE_TRIE__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define PI 					3.14159
#define STATIONARY_ANGLE    1000

#define NO_GESTURE	 	   	-200

#define SEQUENCE_ADDED		3000
#define INVALID_SEQUENCE   	-100

#define ANGLE_PERCENT_ERROR		20
#define LENGTH_PERCENT_ERROR 	20

struct DirectionNode {
    int x;
    int y;
    struct DirectionNode *parent;
    struct ChildNode *children;
    int gesture_code;
};

struct ChildNode {
	struct DirectionNode *direction_node;
	struct ChildNode *next;
};

struct Threshold {
	int radius;
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
struct DirectionNode *nextDirectionNode(struct DirectionNode *next, struct DirectionNode *current, int *gesture_code, struct Threshold *thresh);

/**
 * Adds a gesture sequence to the tree.
 * @param  gesture_code     The gesture code for the given gesture.
 * @param  gesture_sequence An int array with x,y coordinates.
 * @param  n                The number of points in the gesture.
 * @param  thresh 			The angle and length threshold used to compare angles.
 * @return                  Returns 0 if successful and -1 if error.
 */
int addGesture(int gesture_code, int n, int gesture_sequence[n][2], struct Threshold *thresh);

/**
 * @param  x0
 * @param  y0
 * @param  x1
 * @param  y1
 * @param  gesture_code gesture_code should be NO_GESTURE if not a leaf DirectionNode.
 * @return              Returns created DirectionNode.
 */ 
struct DirectionNode *createDirectionNode(int x, int y, int gesture_code);

void printTrie(struct DirectionNode *root);
void printNode(struct DirectionNode *node);

#endif