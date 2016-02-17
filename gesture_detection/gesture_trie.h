#ifndef GESTURE_TRIE__
#define GESTURE_TRIE__

#define UP          		1000
#define DOWN        		1001
#define LEFT        		1002
#define RIGHT       		1003
#define UP_RIGHT    		1004
#define UP_LEFT     		1005
#define DOWN_RIGHT  		1006
#define DOWN_LEFT   		1007
#define STATIONARY     		1008

#define NO_GESTURE	 	   	-200

#define SEQUENCE_ADDED		3000
#define INVALID_SEQUENCE   	-100

struct DirectionNode {
    int direction;
    struct DirectionNode *parent;
    struct ChildNode *children;
    int gesture_code;
};

struct ChildNode {
	struct DirectionNode *direction_node;
	struct ChildNode *next;
};

struct DirectionNode *getBase(void);

// direct is the direction being searched.
// current is the current node.
// gesture_code will be changed to a gesture_code if leaf node found. Otherwise,
// 	it is NO_GESTURE.
// 
// Returns null if DNE.
// Else returns child with correct direction. Will set gesture_code if leaf node.
struct DirectionNode *nextDirectionNode(int direction, struct DirectionNode *current, int *gesture_code);

// Returns 0 if successful.
// Returns -1 if error:
// 	- Gesture is a prefix.
int addGesture(int gesture_code, int gesture_sequence[], int n);

#endif