#include "gesture_trie.h"

#include <stdio.h>

static struct DirectionNode *createDirectionNode(int direction, int gesture_code);
static void addChild(struct DirectionNode *parent, struct DirectionNode *child);
static struct ChildNode *createChildNode(struct DirectionNode *direction_node);

void loadPredefinedGestures(void) {
	int gesture_1[] = {UP, UP, UP_RIGHT, UP};
	int gesture_2[] = {UP, DOWN, DOWN};
	int gesture_3[] = {UP, DOWN, RIGHT, UP};

	addGesture(20, gesture_1, 4);
	addGesture(21, gesture_2, 3);
	addGesture(22, gesture_3, 4);
}

int getDirectionFromCoordinates(int x0, int y0, int x1, int y1, int thresh) {
	int dy = y1 - y0;
	int dx = x1 - x0;

	if (sqrt(pow(dy, 2) + pow(dx, 2)) < thresh) {
		return STATIONARY;
	}

	double shifted_angle = atan2(dy, dx) * 180 / PI + 180;

	if 		(ANG_LEFT_COMPARISON(shifted_angle)) 		return LEFT;
	else if (ANG_DOWN_LEFT_COMPARISON(shifted_angle)) 	return DOWN_LEFT;
	else if (ANG_DOWN_COMPARISON(shifted_angle)) 		return DOWN;
	else if (ANG_DOWN_RIGHT_COMPARISON(shifted_angle)) 	return DOWN_RIGHT;
	else if (ANG_RIGHT_COMPARISON(shifted_angle)) 		return RIGHT;
	else if (ANG_UP_RIGHT_COMPARISON(shifted_angle)) 	return UP_RIGHT;
	else if (ANG_UP_COMPARISON(shifted_angle)) 			return UP;
	else if (ANG_UP_LEFT_COMPARISON(shifted_angle)) 	return UP_LEFT;

	return STATIONARY;
}

struct DirectionNode *getBase(void) {
	static struct DirectionNode *base = NULL;
	if (base == NULL) {
		base = createDirectionNode(STATIONARY, NO_GESTURE);
	}
	return base;
}

// direct is the direction being searched.
// current is the current node.
// gesture_code will be changed to a gesture_code if leaf node found. Otherwise,
// 	it is NO_GESTURE.
// 
// Returns NULL if DNE.
// Else returns child with correct direction. Will set gesture_code if leaf node.
struct DirectionNode *nextDirectionNode(int direction, struct DirectionNode *current, int *gesture_code) {
	struct DirectionNode *search_direction_node;
	*gesture_code = NO_GESTURE;	

	struct ChildNode *search_child_node = current->children;

	while (search_child_node != NULL) {
		search_direction_node = search_child_node->direction_node;

		if (search_direction_node->direction == direction) {
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

// Returns 0 if successful.
// Returns -1 if error:
// 	- Gesture is a prefix.
int addGesture(int gesture_code, int gesture_sequence[], int n) {
	struct DirectionNode *direction_node = getBase();
	int gesture_code_found = NO_GESTURE;

	for (int i=0; i<n; i++) {
		struct DirectionNode *temp_node = nextDirectionNode(gesture_sequence[i], direction_node, &gesture_code_found);

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
		printf("%d -- %d\n", root->direction, root->gesture_code);
	} else {
		printf("%d ", root->direction);
	}

	struct ChildNode *child = root->children;
	while (child != NULL) {
		printTrie(child->direction_node);
		child = child->next;
	}
}

// direction must always be passed in.
// gesture_code should be NO_GESTURE if not a leaf DirectionNode.
static struct DirectionNode *createDirectionNode(int direction, int gesture_code) {
	struct DirectionNode *direction_node = (struct DirectionNode *) malloc(sizeof(struct DirectionNode));

	direction_node->direction = direction;
	direction_node->parent = NULL;
	direction_node->children = NULL;
	direction_node->gesture_code = gesture_code;

	return direction_node;
}

// Private.
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
