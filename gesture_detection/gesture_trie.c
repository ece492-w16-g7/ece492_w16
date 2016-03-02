#include "gesture_trie.h"

static int getLengthFromCoordinates(int x0, int y0, int x1, int y1);
static int compareTwoDirectionNodes(struct DirectionNode *node0, struct DirectionNode *node1, struct Threshold *thresh);
static void addChild(struct DirectionNode *parent, struct DirectionNode *child);
static struct ChildNode *createChildNode(struct DirectionNode *direction_node);

/**
 * Returns base of tree. Note that this stores a static variable.
 * @return  Returns pointer to base of tree.
 */
struct DirectionNode *getBase(void) {
	static struct DirectionNode *base = NULL;
	if (base == NULL) {
		base = createDirectionNode(0, 0, NO_GESTURE);
	}
	return base;
}

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
struct DirectionNode *nextDirectionNode(struct DirectionNode *next, struct DirectionNode *current, int *gesture_code, struct Threshold *thresh) {
	struct DirectionNode *search_direction_node;
	*gesture_code = NO_GESTURE;	

	struct ChildNode *search_child_node = current->children;

	while (search_child_node != NULL) {
		search_direction_node = search_child_node->direction_node;

		if (compareTwoDirectionNodes(search_direction_node, next, thresh) == 0) {
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
 * @param  gesture_code     The gesture code for the given gesture. Please make the
 							first point passed in (0,0).
 * @param  gesture_sequence An int array with x,y coordinates.
 * @param  n                The number of points in the gesture.
 * @param  thresh 			The angle and length threshold used to compare angles.
 * @return                  Returns 0 if successful and -1 if error.
 */
int addGesture(int gesture_code, int n, int gesture_sequence[n][2], struct Threshold *thresh) {
	struct DirectionNode *direction_node, *incoming_node, *search_node;
	int gesture_code_found = NO_GESTURE;

	direction_node = getBase();

	for (int i=0; i<n; i++) {
		incoming_node = createDirectionNode(gesture_sequence[i][0], gesture_sequence[i][1], NO_GESTURE);
		search_node = nextDirectionNode(incoming_node, direction_node, &gesture_code_found, thresh);

		// printf("- (%d,%d) (%d,%d) %d %d\n", gesture_sequence[i-1][0], gesture_sequence[i-1][1], gesture_sequence[i][0], gesture_sequence[i][1], incoming_node->angle, incoming_node->length);

		if (search_node != NULL) {
			// Direction already exists in tree.
			if (gesture_code_found != NO_GESTURE) {
				// gesture_sequence has a prefix already in tree. Invalid.
				return INVALID_SEQUENCE;
			}
			direction_node = search_node;

		} else {
			// New node needs to be created.
			addChild(direction_node, incoming_node);
			direction_node = incoming_node;
		}

		// printf("%d,%d,%d,%d,%d,h\n", i, gesture_sequence[i][0], gesture_sequence[i][1], direction_node->angle, direction_node->length);
	}

	// Last leaf node so we change gesture_code.
	direction_node->gesture_code = gesture_code;

	return SEQUENCE_ADDED;
}

/**
 * @param  x0
 * @param  y0
 * @param  x1
 * @param  y1
 * @param  gesture_code gesture_code should be NO_GESTURE if not a leaf DirectionNode.
 * @return              Returns created DirectionNode.
 */ 
struct DirectionNode *createDirectionNode(int x, int y, int gesture_code) {
	struct DirectionNode *direction_node = (struct DirectionNode *) malloc(sizeof(struct DirectionNode));

	direction_node->x = x;
	direction_node->y = y;
	direction_node->parent = NULL;
	direction_node->children = NULL;
	direction_node->gesture_code = gesture_code;

	return direction_node;
}

void printTrie(struct DirectionNode *root) {
	if (root->gesture_code != NO_GESTURE) { 
		printf("(%d,%d) -- %d\n", root->x, root->y, root->gesture_code);
	} else {
		printf("(%d,%d) ", root->x, root->y);
	}

	struct ChildNode *child = root->children;
	while (child != NULL) {
		printTrie(child->direction_node);
		child = child->next;
	}
}

void printNode(struct DirectionNode *node) {
	printf("(%d,%d)\n", node->x, node->y);
}

static int getLengthFromCoordinates(int x0, int y0, int x1, int y1) {
	int dy = y1 - y0;
	int dx = x1 - x0;

	return (int) (sqrt(pow(dx, 2) + pow(dy, 2)));
}

/**
 * Compares two angles using angle_thresh.
 * @param  node0         
 * @param  node1         
 * @param  thresh 	The angle and length thresholds are used to compare nodes.
 * @return          Returns 0 if angles are similar. Otherwise, 1.
 */
static int compareTwoDirectionNodes(struct DirectionNode *node0, struct DirectionNode *node1, struct Threshold *thresh) {
	int length = getLengthFromCoordinates(node0->x, node0->y, node1->x, node1->y);

	int comparison;

	if (length < thresh->radius) {
		comparison = 0;
	} else {
		comparison = 1;
	}

	return comparison;
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