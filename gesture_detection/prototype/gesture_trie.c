#include "gesture_trie.h"

static int getGridNumFromCoordinates(int x, int y);
static int getColFromGridNum(int grid_num);
static int getRowFromGridNum(int grid_num);

static void printDirectionNode(struct DirectionNode *root);

static int compareFourDirectionNodes(struct DirectionNode *grid0_start, struct DirectionNode *grid0_end, struct DirectionNode *grid1_start, struct DirectionNode *grid1_end);

static void addDirectionNodeChild(struct DirectionNode *parent, struct DirectionNode *child);
static struct ChildDirectionNode *createChildDirectionNode(struct DirectionNode *direction_node);
static void addSearchNode(struct DirectionNode *first, struct DirectionNode *second);
static struct SearchNode *createSearchNode(struct DirectionNode *first, struct DirectionNode *second);


struct DirectionNode *getDummyBase(void) {
	struct SearchNode *base = getBase();
	return base->second;
}

/**
 * Returns base of tree. Note that this stores a static variable.
 * @return  Returns pointer to base of tree.
 */
struct SearchNode *getBase(void) {
	static struct SearchNode *base = NULL;
	if (base == NULL) {
		struct DirectionNode *dummy = createDirectionNode(0, 0, NO_GESTURE);
		base = createSearchNode(dummy, dummy);
	}
	return base;
}

/**
 * Finds the first direction node for a gesture.
 * @param  current The node that is searched for.
 * @param  last    The last node.
 * @return         Returns NULL if DNE. Else returns DirectionNode.
 */
struct DirectionNode *firstDirectionNode(struct DirectionNode *current, struct DirectionNode *last) {
	struct SearchNode *base = getBase();

	while (base != NULL) {
		if (compareFourDirectionNodes(base->first, base->second, last, current) == NODES_SAME) {
			return base->second;
		}

		base = base->next;
	}

	return NULL;
}

/**
 * Finds the next node to move to given an angle and the current direction.
 * @param  current     	The node that is searched for.
 * @param  base      	The current base.
 * @param  last 		The last node.
 * @param  gesture_code Pointer to an int for returning gesture_code. Set to gesture_code
 *                      only if a leaf node is found. Otherwise, set to NO_GESTURE.
 * @return              Returns NULL if DNE. Else, returns child. Will
 *                      set gesture_code if leaf node.
 */
struct DirectionNode *nextDirectionNode(struct DirectionNode *current, struct DirectionNode *base, struct DirectionNode *last, int *gesture_code) {
	struct DirectionNode *search_direction_node;
	*gesture_code = NO_GESTURE;	

	struct ChildDirectionNode *search_child_node = base->children;

	while (search_child_node != NULL) {
		search_direction_node = search_child_node->direction_node;

		if (compareFourDirectionNodes(base, search_direction_node, last, current) == NODES_SAME) {
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
int addGesture(int gesture_code, int n, int gesture_sequence[n][2]) {
	struct DirectionNode *direction_node, *incoming_node, *last_incoming_node, *search_node;
	int gesture_code_found = NO_GESTURE;

	if (n < 2) {
		// Too small.
		return INVALID_SEQUENCE;
	}

	struct DirectionNode *first = createDirectionNode(gesture_sequence[0][0], gesture_sequence[0][1], NO_GESTURE);
	struct DirectionNode *second = createDirectionNode(gesture_sequence[1][0], gesture_sequence[1][1], NO_GESTURE);

	int i = 2;
	while ((i < n) && (compareTwoDirectionNodes(first, second) == NODES_SAME)) {
		free(second);
		second = createDirectionNode(gesture_sequence[i][0], gesture_sequence[i][1], NO_GESTURE);
		i++;
	}

	last_incoming_node = firstDirectionNode(second, first);
	if (last_incoming_node == NULL) {
		addSearchNode(first, second);
		last_incoming_node = second;
		direction_node = second;
	} else {
		direction_node = last_incoming_node;
		free(first);
		free(second);
	}

	for (i=i; i<n; i++) {
		incoming_node = createDirectionNode(gesture_sequence[i][0], gesture_sequence[i][1], NO_GESTURE);

		if (compareTwoDirectionNodes(incoming_node, last_incoming_node) == NODES_SAME) {
			free(incoming_node);
			continue;
		}

		search_node = nextDirectionNode(incoming_node, direction_node, last_incoming_node, &gesture_code_found);

		if (search_node != NULL) {
			// Direction already exists in tree.
			if (gesture_code_found != NO_GESTURE) {
				// gesture_sequence has a prefix already in tree. Invalid.
				return INVALID_SEQUENCE;
			}

			free(last_incoming_node);
			direction_node = search_node;

		} else {
			// New node needs to be created.
			addDirectionNodeChild(direction_node, incoming_node);
			direction_node = incoming_node;
		}

		last_incoming_node = incoming_node;
	}

	// Last leaf node so we change gesture_code.
	direction_node->gesture_code = gesture_code;

	return SEQUENCE_ADDED;
}

/**
 * @param  x
 * @param  y
 * @param  gesture_code gesture_code should be NO_GESTURE if not a leaf DirectionNode.
 * @return              Returns created DirectionNode.
 */ 
struct DirectionNode *createDirectionNode(int x, int y, int gesture_code) {
	int grid_num = getGridNumFromCoordinates(x, y);

	struct DirectionNode *direction_node = (struct DirectionNode *) malloc(sizeof(struct DirectionNode));

	direction_node->grid_num = grid_num;
	direction_node->parent = NULL;
	direction_node->children = NULL;
	direction_node->gesture_code = gesture_code;

	return direction_node;
}

void printStorage(struct SearchNode *root) {
	struct SearchNode *base = root;
	while (base != NULL) {
		printDirectionNode(base->first);
		fflush(stdout);
		printDirectionNode(base->second);
		fflush(stdout);
		base = base->next;
	}
}

static void printDirectionNode(struct DirectionNode *root) {
	if (root->gesture_code != NO_GESTURE) { 
		printf("(%d) -- %d\n", root->grid_num, root->gesture_code);
	} else {
		printf("(%d) ", root->grid_num);
	}

	struct ChildDirectionNode *child = root->children;
	while (child != NULL) {
		printDirectionNode(child->direction_node);
		child = child->next;
	}
}

static int getGridNumFromCoordinates(int x, int y) {
	int col = x / GRID_SIZE;
	int row = y / GRID_SIZE;

	return row * GRID_TOT_COL + col;
}

static int getColFromGridNum(int grid_num) {
	return grid_num % GRID_TOT_COL;
}

static int getRowFromGridNum(int grid_num) {
	return grid_num / GRID_TOT_COL;
}

/**
 * Compares two angles using angle_thresh.
 * @param  node0         
 * @param  node1         
 * @param  thresh 	The angle and length thresholds are used to compare nodes.
 * @return          Returns 0 if angles are similar. Otherwise, 1.
 */
int compareTwoDirectionNodes(struct DirectionNode *node0, struct DirectionNode *node1) {
	int col0 = getColFromGridNum(node0->grid_num);
	int row0 = getRowFromGridNum(node0->grid_num);
	int col1 = getColFromGridNum(node1->grid_num);
	int row1 = getRowFromGridNum(node1->grid_num);

	int comparison;

	if ((abs(col0 - col1) < GRID_NEIGHBOURS_THRESH) 
		&& (abs(row0 - row1) < GRID_NEIGHBOURS_THRESH)) {
		comparison = NODES_SAME;
	} else {
		comparison = NODES_DIFFERENT;
	}

	return comparison;
}

static int compareFourDirectionNodes(struct DirectionNode *grid0_start, struct DirectionNode *grid0_end, struct DirectionNode *grid1_start, struct DirectionNode *grid1_end) {
	if ((grid1_start->grid_num == 0) && (grid0_start->grid_num == 0)) {
		return NODES_SAME;
	}

	int dy_0 = getRowFromGridNum(grid0_end->grid_num) - getRowFromGridNum(grid0_start->grid_num);
	int dx_0 = getColFromGridNum(grid0_end->grid_num) - getColFromGridNum(grid0_start->grid_num);

	int dy_1 = getRowFromGridNum(grid1_end->grid_num) - getRowFromGridNum(grid1_start->grid_num);
	int dx_1 = getColFromGridNum(grid1_end->grid_num) - getColFromGridNum(grid1_start->grid_num);

	int comparison;

	if ((abs(dy_0 - dy_1) < GRID_DIFF_THRESH) 
		&& (abs(dx_0 - dx_1) < GRID_DIFF_THRESH)) {
		comparison = NODES_SAME;
	} else {
		comparison = NODES_DIFFERENT;
	}

	return comparison;
}

static void addDirectionNodeChild(struct DirectionNode *parent, struct DirectionNode *child) {
	if (parent->children != NULL) {
		
		struct ChildDirectionNode *child_node = parent->children;
		while (child_node->next != NULL) {
			child_node = child_node->next;
		}

		child_node->next = createChildDirectionNode(child);

	} else {
		parent->children = createChildDirectionNode(child);
	}

	child->parent = parent;
}

static struct ChildDirectionNode *createChildDirectionNode(struct DirectionNode *direction_node) {
	struct ChildDirectionNode *child_node = (struct ChildDirectionNode *) malloc(sizeof(struct ChildDirectionNode));

	child_node->direction_node = direction_node;
	child_node->next = NULL;

	return child_node;
}

static void addSearchNode(struct DirectionNode *first, struct DirectionNode *second) {
	struct SearchNode *base = getBase();

	while (base->next != NULL) {
		if (compareFourDirectionNodes(base->first, base->second, first, second) == NODES_SAME) {
			return;
		}

		base = base->next;
	}

	if (compareFourDirectionNodes(base->first, base->second, first, second) == NODES_DIFFERENT) {
		base->next = createSearchNode(first, second);
	}
}

static struct SearchNode *createSearchNode(struct DirectionNode *first, struct DirectionNode *second) {
	struct SearchNode *node = (struct SearchNode *) malloc(sizeof(struct SearchNode));

	node->first = first;
	node->second = second;
	node->next = NULL;

	return node;
}