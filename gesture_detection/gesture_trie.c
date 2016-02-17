#include "gesture_trie.h"

static struct DirectionNode *createDirectionNode(int direction, int gesture_code);
static void addChild(struct DirectionNode *parent, struct DirectionNode *child);
static struct ChildNode *createChildNode(struct DirectionNode *direction_node);

struct DirectionNode *getBase(void) {
	static struct DirectionNode *base = NULL;
	if (!base) {
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

	while (search_child_node) {
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

	if (!search_child_node) {
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

		if (temp_node) {
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
	if (parent->children) {
		
		struct ChildNode *child_node = parent->children;
		while (child_node->next) {
			child_node = child_node->next;
		}

		child_node->next = createChildNode(child);

	} else {
		parent->children = createChildNode(child);
	}
}

static struct ChildNode *createChildNode(struct DirectionNode *direction_node) {
	struct ChildNode *child_node = (struct ChildNode *) malloc(sizeof(struct ChildNode));

	child_node->direction_node = direction_node;
	child_node->next = NULL;

	return child_node;
}