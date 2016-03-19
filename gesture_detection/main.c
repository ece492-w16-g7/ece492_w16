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
#include <stdlib.h>
#include "includes.h"
#include "system.h"
#include "io.h"
#include "altera_up_avalon_character_lcd.h"

#include "gesture_trie.h"
#include "gesture_storage.h"

/* Generic buffer sizes. */
#define SMALL_BUF_SIZE			20
#define LARGE_BUF_SIZE 			100

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048

OS_STK    GestureRecognitionTask_stk[TASK_STACKSIZE];
OS_STK    LCDTask_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */
#define LCD_TASK_PRIORITY					1
#define GESTURE_RECOGNITION_TASK_PRIORITY	2

/* Screen Definitions */
#define UPPER_SCREEN             0
#define LOWER_SCREEN             1
#define M_POSITION               0
#define N_POSITION               8
#define INIT_POSITION            0
#define SCREEN_ROW_SIZE			16

/* Globals */
alt_up_character_lcd_dev * char_lcd_dev;

#define RECOGNITION_ON

/* Queue for LCD to print message. */
#define LCDQSize 2
OS_EVENT *lcd_queue;
void *LCDQueue[LCDQSize];

/* Queue for position data. */
#define PositionQSize 2
OS_EVENT *position_queue;
void *PositionQueue[PositionQSize];

static void irq_handler (void * context) {
	int centroid = IORD_32DIRECT(LED_DETECTOR_BASE, 0);
	OSQPost(position_queue, (void *) centroid);
}

void gesture_recognition_task(void *pdata)
{
	INT8U err;

	double end_time = 0.0;
	double start_time = OSTimeGet();
	double frame_rate = 0.0;
	double time_passed = 0.0;

	int frame_count = 0;
	int x = 0;
	int y = 0;
	int direction_code = 0;

	struct DirectionNode *current = getBase();
	printTrie(current);

	struct DirectionNode *incoming_node, *last_incoming_node = getBase();

	while (1) {

		int centroid = (int) OSQPend(position_queue, 0, &err);
		if (err == OS_NO_ERR) {
			end_time = OSTimeGet();
			frame_count += 1;
			time_passed = (end_time - start_time) / OS_TICKS_PER_SEC;

			x = centroid & 1023;
			y = centroid >> 16;
			frame_rate = frame_count / time_passed;

			char *message = (char *) malloc(LARGE_BUF_SIZE * sizeof(char));
//			snprintf(message, LARGE_BUF_SIZE, "M:%d N:%d f:%2.0f n:%d", x, y, frame_rate, frame_count);

			#ifdef RECOGNITION_ON
			incoming_node = createDirectionNode(x, y, NO_GESTURE);
//			snprintf(message, LARGE_BUF_SIZE, "%d,%d,%d,%d", frame_count, x, y, incoming_node->grid_num);

			if (compareTwoDirectionNodes(incoming_node, last_incoming_node) == NODES_DIFFERENT) {

				current = nextDirectionNode(incoming_node, current, last_incoming_node, &direction_code);
				if (current) {
					// Leaf node
					if (current->gesture_code != NO_GESTURE) {
//						snprintf(message, LARGE_BUF_SIZE, "%s #%d", message, current->gesture_code);
						snprintf(message, LARGE_BUF_SIZE, "%d - #%d", frame_count, current->gesture_code);
						current = getBase();
						last_incoming_node = getBase();

						if (OSQPost(lcd_queue, message) != OS_NO_ERR)
							printf("Error: message not put on LCD queue.\n");

					} else {
						last_incoming_node = incoming_node;
//						snprintf(message, LARGE_BUF_SIZE, "%s h", message);
					}
				} else {
					current = getBase();
					last_incoming_node = getBase();
//					snprintf(message, LARGE_BUF_SIZE, "%s m", message);
				}
			} else {
//				snprintf(message, LARGE_BUF_SIZE, "%s c", message);
			}
			#endif

//		printf("%s\n", message);

//		if (OSQPost(lcd_queue, message) != OS_NO_ERR)
//			printf("Error: message not put on LCD queue.\n");

		} else {
			printf("Error receiving centroid!\n");
		}
	}
}

void LCD_task(void* pdata)
{
	INT8U err;

	// Open the Character LCD Port
	char_lcd_dev = alt_up_character_lcd_open_dev(CHARACTER_LCD_0_NAME);

	if(char_lcd_dev == NULL)
	  printf("Error: Could not open character LCD device\n");
	else
	  printf("Success: Character LCD device opened\n");

	/*Initialize the character display*/
	alt_up_character_lcd_init(char_lcd_dev);
	alt_up_character_lcd_cursor_off(char_lcd_dev);

	char upper_buf[SMALL_BUF_SIZE];
	char lower_buf[SMALL_BUF_SIZE];

	while (1)
	{
		char *message = (char *) OSQPend(lcd_queue, 0, &err);
		if (err != OS_NO_ERR) {
			printf("Error: message not received.\n");
		}

		snprintf(upper_buf, SCREEN_ROW_SIZE, "%s", message);
		snprintf(lower_buf, SCREEN_ROW_SIZE, "%s", message + SCREEN_ROW_SIZE - 1);

		alt_up_character_lcd_init(char_lcd_dev);

		alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, UPPER_SCREEN);
		alt_up_character_lcd_string(char_lcd_dev, upper_buf);

		alt_up_character_lcd_set_cursor_pos(char_lcd_dev, 0, LOWER_SCREEN);
		alt_up_character_lcd_string(char_lcd_dev, lower_buf);

		free(message);
	}
}

/* The main function creates two task and starts multi-tasking */
int main(void)
{
	INT8U err;

	lcd_queue = OSQCreate(LCDQueue, LCDQSize);
	if (lcd_queue == NULL)
		printf("Error: could not create LCD queue\n");

	position_queue = OSQCreate(PositionQueue, PositionQSize);
	if (position_queue == NULL)
		printf("Error: could not create position queue\n");

	/* Initiate Interrupt */
	alt_ic_isr_register(LED_DETECTOR_IRQ_INTERRUPT_CONTROLLER_ID,
			LED_DETECTOR_IRQ,
			irq_handler,
			NULL,
			NULL);

	err = OSTaskCreateExt(LCD_task,
				  NULL,
				  (void *)&LCDTask_stk[TASK_STACKSIZE-1],
				  LCD_TASK_PRIORITY,
				  LCD_TASK_PRIORITY,
				  LCDTask_stk,
				  TASK_STACKSIZE,
				  NULL,
				  0);

	if (err != OS_NO_ERR)
		printf("LCD task not created.\n");

	err = OSTaskCreateExt(gesture_recognition_task,
				  NULL,
				  (void *)&GestureRecognitionTask_stk[TASK_STACKSIZE-1],
				  GESTURE_RECOGNITION_TASK_PRIORITY,
				  GESTURE_RECOGNITION_TASK_PRIORITY,
				  GestureRecognitionTask_stk,
				  TASK_STACKSIZE,
				  NULL,
				  0);

	if (err != OS_NO_ERR)
		printf("gesture recognition task not created.\n");

	storeAllPredefinedGestures();

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
