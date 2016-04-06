/**
 * ECE492 - Group 7 - Winter 2016
 *
 * Description: This is the file that stores hardcoded gesture
 * 				values. This is essentially storage.
 * Author: Shahzeb Asif
 * Date: Apr 5, 2016
 *
 */

#include "gesture_storage.h"

static void storeSimpleDownRight(void) {
	int gesture_code = GCODE_DOWN_RIGHT;
	int n = 4;
	int gesture[4][2] = {{320,30},{240,60},{160,90},{80,120}};

	addGesture(gesture_code, n, gesture);
}

static void storeSimpleUpLeft(void) {
	int gesture_code = GCODE_UP_LEFT;
	int n = 4;
	int gesture[4][2] = {{80,120},{160,90},{240,60},{320,30}};

	addGesture(gesture_code, n, gesture);
}

static void storeSimpleDownLeft(void) {
	int gesture_code = GCODE_DOWN_LEFT;
	int n = 4;
	int gesture[4][2] = {{80,30},{160,60},{240,90},{320,120}};

	addGesture(gesture_code, n, gesture);
}

static void storeSimpleUpRight(void) {
	int gesture_code = GCODE_UP_RIGHT;
	int n = 4;
	int gesture[4][2] = {{320,120},{240,90},{160,60},{80,30}};

	addGesture(gesture_code, n, gesture);
}

void storeAllPredefinedGestures() {
	storeSimpleDownLeft();
	storeSimpleDownRight();
	storeSimpleUpLeft();
	storeSimpleUpRight();
}
