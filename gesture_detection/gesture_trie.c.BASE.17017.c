#include <stdio.h>
#include <stdlib.h>

#define UP          1000
#define DOWN        1001
#define LEFT        1002
#define RIGHT       1003
#define UP_RIGHT    1004
#define UP_LEFT     1005
#define DOWN_RIGHT  1006
#define DOWN_LEFT   1007
#define NO_MOVE     1008

struct Node {
    int direction;
    struct Node *parent;
    struct Node *children;
    int gesture_code;
};

// Public.
struct Node *getBase(void) {
}

// Public.
// Returns current if at leaf node.
// Returns null if DNE.
// Else returns child with correct direction.
struct Node *nextNode(int direction, struct Node *current) {
}

// Public.
// Returns -1 if node is not leaf node.
// Else returns gesture_code.
int getGestureCode(struct Node *node) {
}

// Public.
int addGesture(int gesture_code, char *gesture_string) {
}

// Private.
void createNode(int direction, struct Node *parent) {
}

// Private.
void addChild(struct Node *parent, struct Node *child) {
}


