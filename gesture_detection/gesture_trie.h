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

struct DirectionNode {
    int angle;
    struct DirectionNode *parent;
    struct ChildNode *children;
    int gesture_code;
};

struct ChildNode {
	struct DirectionNode *direction_node;
	struct ChildNode *next;
};

/**
 * Finds angle between two points.
 * @param  x0            
 * @param  y0            
 * @param  x1            
 * @param  y1            
 * @param  length_thresh If the length between two points is less than the
 *                       length_thresh, then the angle returned is a special
 *                       angle to signify that the points are too close together
 *                       for the actual angle to matter.
 * @return               Returns angle between points or special angle if the
 *                       length between points is less than length_thresh.
 */
int getAngleFromCoordinates(int x0, int y0, int x1, int y1, int length_thresh);

/**
 * Returns base of tree. Note that this stores a static variable.
 * @return  Returns pointer to base of tree.
 */
struct DirectionNode *getBase(void);

/**
 * Finds the next node to move to given an angle and the current direction.
 * @param  angle        The angle for the searching node.
 * @param  current      The current node.
 * @param  gesture_code Pointer to an int for returning gesture_code. Set to gesture_code
 *                      only if a leaf node is found. Otherwise, set to NO_GESTURE.
 * @param  angle_thresh The angle threshold used to compare angles.
 * @return              Returns NULL if DNE. Else, returns child with correct angle. Will
 *                      set gesture_code if leaf node.
 */
struct DirectionNode *nextDirectionNode(int angle, struct DirectionNode *current, int *gesture_code, int angle_thresh);

/**
 * Adds a gesture sequence to the tree.
 * @param  gesture_code     The gesture code for the given gesture.
 * @param  gesture_sequence An int array with angles. Please keep angles >= 0 and < 359.
 *                          Note that there is a special STATIONARY_ANGLE as well.
 * @param  n                The number of points in the gesture.
 * @param angle_thresh		Used to compare to existing gestures in the tree.
 * @return                  Returns 0 if successful and -1 if error.
 */
int addGesture(int gesture_code, int gesture_sequence[], int n, int angle_thresh);
	
void printTrie(struct DirectionNode *root);

#endif