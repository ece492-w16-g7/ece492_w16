#include "gesture_trie.h"

static int compareTwoAngles(int ang0, int ang1, int angle_thresh);
static struct DirectionNode *createDirectionNode(int angle, int gesture_code);
static void addChild(struct DirectionNode *parent, struct DirectionNode *child);
static struct ChildNode *createChildNode(struct DirectionNode *direction_node);

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
int getAngleFromCoordinates(int x0, int y0, int x1, int y1, int length_thresh) {
	int dy = y1 - y0;
	int dx = x1 - x0;

	int angle = 0;

	if (sqrt(pow(dy, 2) + pow(dx, 2)) < length_thresh) {
		angle = STATIONARY_ANGLE;
	} else {
		// Assuming 0/2PI on left and PI on right.
		angle = (int) (atan2(dy, dx) * 180 / PI + 180);
	}

	return angle;
}



/**
 * Returns base of tree. Note that this stores a static variable.
 * @return  Returns pointer to base of tree.
 */
struct DirectionNode *getBase(void) {
	static struct DirectionNode *base = NULL;
	if (base == NULL) {
		base = createDirectionNode(STATIONARY_ANGLE, NO_GESTURE);
	}
	return base;
}

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
struct DirectionNode *nextDirectionNode(int angle, struct DirectionNode *current, int *gesture_code, int angle_thresh) {
	struct DirectionNode *search_direction_node;
	*gesture_code = NO_GESTURE;	

	struct ChildNode *search_child_node = current->children;

	while (search_child_node != NULL) {
		search_direction_node = search_child_node->direction_node;

		if (compareTwoAngles(search_direction_node->angle, angle, angle_thresh) == 0) {
			if (search_direction_node->gesture_code != NO_GESTURE) {
				*gesture_code = search_direction_node->gesture_code;
			}
			
			break;

		} else {
			search_child_node = search_child_node->next;
		}
	}

	if (search_child_node == NULL) {
		search_direction_node = NULL;
	}

	return search_direction_node;
}

/**
 * Adds a gesture sequence to the tree.
 * @param  gesture_code     The gesture code for the given gesture.
 * @param  gesture_sequence An int array with angles. Please keep angles >= 0 and < 359.
 *                          Note that there is a special STATIONARY_ANGLE as well.
 * @param  n                The number of points in the gesture.
 * @param angle_thresh		Used to compare to existing gestures in the tree.
 * @return                  Returns 0 if successful and -1 if error.
 */
int addGesture(int gesture_code, int gesture_sequence[], int n, int angle_thresh) {
	struct DirectionNode *direction_node = getBase();
	int gesture_code_found = NO_GESTURE;

	for (int i=0; i<n; i++) {
		struct DirectionNode *temp_node = nextDirectionNode(gesture_sequence[i], direction_node, &gesture_code_found, angle_thresh);

		if (temp_node != NULL) {
			// Direction already exists in tree.
			if (gesture_code_found != NO_GESTURE) {
				// gesture_sequence has a prefix already in tree. Invalid.
				return INVALID_SEQUENCE;
			}
			direction_node = temp_node;

		} else {
			// New node needs to be created.
			temp_node = createDirectionNode(gesture_sequence[i], NO_GESTURE);
			addChild(direction_node, temp_node);
			direction_node = temp_node;
		}
	}

	// Last leaf node so we change gesture_code.
	direction_node->gesture_code = gesture_code;

	return SEQUENCE_ADDED;
}

void printTrie(struct DirectionNode *root) {
	if (root->gesture_code != NO_GESTURE) { 
		printf("%d -- %d\n", root->angle, root->gesture_code);
	} else {
		printf("%d ", root->angle);
	}

	struct ChildNode *child = root->children;
	while (child != NULL) {
		printTrie(child->direction_node);
		child = child->next;
	}
}

/**
 * Compares two angles using angle_thresh.
 * @param  ang0         
 * @param  ang1         
 * @param  angle_thresh The +/- angle_thresh used to check if the two
 *                      angles are similar.
 * @return              Returns 0 if angles are similar. Otherwise, 1.
 */
static int compareTwoAngles(int ang0, int ang1, int angle_thresh) {
	int ang_diff = abs(ang1 - ang0);
	int ang_comparison;

	if (ang_diff < angle_thresh) {
		ang_comparison = 0;
	} else {
		ang_comparison = 1;
	}

	return ang_comparison;
}

/**
 * @param  angle        angle must always be passed in.
 * @param  gesture_code gesture_code should be NO_GESTURE if not a leaf DirectionNode.
 * @return              Returns created DirectionNode.
 */
static struct DirectionNode *createDirectionNode(int angle, int gesture_code) {
	struct DirectionNode *direction_node = (struct DirectionNode *) malloc(sizeof(struct DirectionNode));

	direction_node->angle = angle;
	direction_node->parent = NULL;
	direction_node->children = NULL;
	direction_node->gesture_code = gesture_code;

	return direction_node;
}

static void addChild(struct DirectionNode *parent, struct DirectionNode *child) {
	if (parent->children != NULL) {
		
		struct ChildNode *child_node = parent->children;
		while (child_node->next != NULL) {
			child_node = child_node->next;
		}

		child_node->next = createChildNode(child);

	} else {
		parent->children = createChildNode(child);
	}

	child->parent = parent;
}

static struct ChildNode *createChildNode(struct DirectionNode *direction_node) {
	struct ChildNode *child_node = (struct ChildNode *) malloc(sizeof(struct ChildNode));

	child_node->direction_node = direction_node;
	child_node->next = NULL;

	return child_node;
}
