#include "gesture_trie.h"

static int getGridNumFromCoordinates(int x, int y);
static void addChild(struct DirectionNode *parent, struct DirectionNode *child);
static struct ChildNode *createChildNode(struct DirectionNode *direction_node);
static int compareGridNums(int grid0_start, int grid0_end, int grid1_start, int grid1_end);
static struct DirectionNode *createTrie(void);

/**
 * Returns base of tree. Note that this stores a static variable.
 * @return  Returns pointer to base of tree.
 */
struct DirectionNode *getBase(void) {
	static struct DirectionNode *base = NULL;
	if (base == NULL) {
		base = createTrie();
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
struct DirectionNode *nextDirectionNode(struct DirectionNode *next, struct DirectionNode *current, struct DirectionNode *last, int *gesture_code) {
	struct DirectionNode *search_direction_node;
	*gesture_code = NO_GESTURE;	

	struct ChildNode *search_child_node = current->children;

	while (search_child_node != NULL) {
		search_direction_node = search_child_node->direction_node;

		if (compareGridNums(current->grid_num, search_direction_node->grid_num, last->grid_num, next->grid_num) == NODES_SAME) {
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

	direction_node = getBase();
	last_incoming_node = getBase();

	int i=0;
	for (i=0; i<n; i++) {
		incoming_node = createDirectionNode(gesture_sequence[i][0], gesture_sequence[i][1], NO_GESTURE);

		if (compareTwoDirectionNodes(incoming_node, last_incoming_node) == NODES_SAME) {
			continue;
		}

		search_node = nextDirectionNode(incoming_node, direction_node, last_incoming_node, &gesture_code_found);

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

void printTrie(struct DirectionNode *root) {
	if (root->gesture_code != NO_GESTURE) { 
		printf("(%d) -- %d\n", root->grid_num, root->gesture_code);
	} else {
		printf("(%d) ", root->grid_num);
	}

	struct ChildNode *child = root->children;
	while (child != NULL) {
		printTrie(child->direction_node);
		child = child->next;
	}
}

void printNode(struct DirectionNode *node) {
	printf("(%d)\n", node->grid_num);
}

static struct DirectionNode *createTrie(void) {
	struct DirectionNode *base = createDirectionNode(0, 0, NO_GESTURE);
//	struct DirectionNode *one = createDirectionNode(0, GRID_BOX_LENGTH, NO_GESTURE);
//	struct DirectionNode *two = createDirectionNode(0, 2 * GRID_BOX_LENGTH, NO_GESTURE);
//
//	addChild(base, one);
//	addChild(one, two);

	return base;
}

static int getGridNumFromCoordinates(int x, int y) {
	int col = x / GRID_BOX_WIDTH;
	int row = y / GRID_BOX_LENGTH;

	return row * GRID_TOTAL_COLS + col;
}

static int getColFromGridNum(int grid_num) {
	return grid_num % GRID_TOTAL_COLS;
}

static int getRowFromGridNum(int grid_num) {
	return grid_num / GRID_TOTAL_COLS;
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

static int compareGridNums(int grid0_start, int grid0_end, int grid1_start, int grid1_end) {
	if ((grid1_start == 0) && (grid0_start == 0)) {
		return NODES_SAME;
	}
	int row0end = getRowFromGridNum(grid0_end);
	int col0end = getColFromGridNum(grid0_end);

	int row0start = getRowFromGridNum(grid0_start);
	int col0start = getColFromGridNum(grid0_start);	

	int row1end = getRowFromGridNum(grid1_end);
	int col1end = getColFromGridNum(grid1_end);

	int row1start = getRowFromGridNum(grid1_start);
	int col1start = getColFromGridNum(grid1_start);

	// int dy_0 = getRowFromGridNum(grid0_end) - getRowFromGridNum(grid0_start);
	// int dx_0 = getColFromGridNum(grid0_end) - getColFromGridNum(grid0_start);

	// int dy_1 = getRowFromGridNum(grid1_end) - getRowFromGridNum(grid1_start);
	// int dx_1 = getColFromGridNum(grid1_end) - getColFromGridNum(grid1_start);
	
	int dy_0 = row0end - row0start;
	int dx_0 = col0end - col0start;
	int dy_1 = row1end - row1start;
	int dx_1 = col1end - col1start;

	int comparison;

	if ((abs(dy_0 - dy_1) < GRID_DIFF_THRESH) 
		&& (abs(dx_0 - dx_1) < GRID_DIFF_THRESH)) {
		comparison = NODES_SAME;
	} else {
		comparison = NODES_DIFFERENT;
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
