/*************************************************************************
 * Copyright (c) 2004 Altera Corporation, San Jose, California, USA.      *
 * All rights reserved. All use of this software and documentation is     *
 * subject to the License Agreement located at the end of this file below.*
 **************************************************************************
 * Description:                                                           *
 * The following is a simple hello world program running MicroC/OS-II.The *
 * purpose of the design is to be a very simple application that just     *
 * demonstrates MicroC/OS-II running on NIOS II.The design doesn't account*
 * for issues such as checking system call return codes. etc.             *
 *                                                                        *
 * Requirements:                                                          *
 *   -Supported Example Hardware Platforms                                *
 *     Standard                                                           *
 *     Full Featured                                                      *
 *     Low Cost                                                           *
 *   -Supported Development Boards                                        *
 *     Nios II Development Board, Stratix II Edition                      *
 *     Nios Development Board, Stratix Professional Edition               *
 *     Nios Development Board, Stratix Edition                            *
 *     Nios Development Board, Cyclone Edition                            *
 *   -System Library Settings                                             *
 *     RTOS Type - MicroC/OS-II                                           *
 *     Periodic System Timer                                              *
 *   -Know Issues                                                         *
 *     If this design is run on the ISS, terminal output will take several*
 *     minutes per iteration.                                             *
 **************************************************************************/


#include <stdio.h>
#include "includes.h"
#include "altera_up_avalon_character_lcd.h"

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048

OS_STK    GestureRecognition_stk[TASK_STACKSIZE];
OS_STK 	  LCDTask_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */

#define GestureRecognitionTask_PRIORITY	1
#define AddGestureTask_PRIORITY         2
#define MusicPlayerTask_PRIORITY        3
#define LCDTask_PRIORITY                4

// Queue for lcd to print message after
// gesture recognition.
#define LCDQSize 2
OS_EVENT *lcd_queue;
void *LCDQueue[LCDQSize];

// Queue for gesture recognition.
#define GesQSize 10
OS_EVENT *gesture_queue;
void *GesQueue[GesQSize];

// Queue for music player commands.
#define MusicQSize 4
OS_EVENT *music_player_queue;
void *MusicQueue[MusicQSize];

alt_up_character_lcd_dev * char_lcd_dev;

// A state-machine-esque task that will use
// gesture_trie.c to recognize gestures.
void GestureRecognitionTask(void* pdata) {
}

void AddGestureTask(void* pdata) {
}

void MusicPlayerTask(void* pdata) {
}

void LCDTask(void* pdata) {
}

// Should be called at regular intervals to post
// new direction to a queue.
static void direction_posting_isr(void *context) {
}

static void add_gesture_isr(void *context) {
}

/* The main function creates three tasks and starts multi-tasking */
int main(void) {
	OSStart();
	return 0;
}

/******************************************************************************
 *                                                                             *
 * License Agreement                                                           *
 *                                                                             *
 * Copyright (c) 2004 Altera Corporation, San Jose, California, USA.           *
 * All rights reserved.                                                        *
 *                                                                             *
 * Permission is hereby granted, free of charge, to any person obtaining a     *
 * copy of this software and associated documentation files (the "Software"),  *
 * to deal in the Software without restriction, including without limitation   *
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
 * and/or sell copies of the Software, and to permit persons to whom the       *
 * Software is furnished to do so, subject to the following conditions:        *
 *                                                                             *
 * The above copyright notice and this permission notice shall be included in  *
 * all copies or substantial portions of the Software.                         *
 *                                                                             *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
 * DEALINGS IN THE SOFTWARE.                                                   *
 *                                                                             *
 * This agreement shall be governed in all respects by the laws of the State   *
 * of California and by the laws of the United States of America.              *
 * Altera does not recommend, suggest or require that this reference design    *
 * file be used in conjunction or combination with any other product.          *
 ******************************************************************************/
