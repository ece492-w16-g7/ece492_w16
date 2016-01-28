#include <stdio.h>
#include <stdlib.h>

#define UP      1000
#define DOWN    1001
#define LEFT    1002
#define RIGHT   1003

struct Node {
    int direction;
    struct Node *parent;
    struct Node *children;
    int gesture_code;
};

struct Node *getBase(void) {
}

// Returns current if at leaf node.
// Returns null if DNE.
// Else returns child with correct direction.
struct Node *nextNode(int direction, struct Node *current) {
}

// Returns -1 if node is not leaf node.
// Else returns gesture_code.
int getGestureCode(struct Node *node) {
}

void createNode(int direction, struct Node *parent) {
}

void addChild(struct Node *parent, struct Node *child) {
}


