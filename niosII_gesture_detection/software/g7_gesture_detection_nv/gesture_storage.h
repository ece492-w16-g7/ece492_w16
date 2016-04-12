/**
 * ECE492 - Group 7 - Winter 2016
 *
 * Description: This is the header file for gesture_storage.c
 * Author: Shahzeb Asif
 * Date: Apr 5, 2016
 *
 */

#ifndef GESTURE_STORAGE__
#define GESTURE_STORAGE__

#include <stdio.h>
#include "gesture_trie.h"

/* Randomly assigned codes to each of the gestures. */
#define GCODE_UP_RIGHT 		10
#define GCODE_UP_LEFT 		20
#define GCODE_DOWN_LEFT 	30
#define GCODE_DOWN_RIGHT 	40

void storeAllPredefinedGestures();

#endif
