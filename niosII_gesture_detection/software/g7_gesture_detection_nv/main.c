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
/**
 * ECE492 - Group 7 - Winter 2016
 *
 * Description: This is the main.c file for the gesture detection project.
 * 				It contains all the tasks required for the project to work.
 * Author: Patrick Kuczera, Andrew Zhong, Shahzeb Asif
 * Date: Apr 5, 2016
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include "includes.h"
#include "system.h"
#include "io.h"
#include "sys/alt_irq.h"
#include "altera_up_avalon_character_lcd.h"

#include "altera_up_avalon_audio.h"
#include "altera_up_avalon_audio_regs.h"
#include "altera_up_avalon_audio_and_video_config.h"
#include "sd/file.h"
#include "sd/efs.h"

#include "gesture_trie.h"
#include "gesture_storage.h"

/* Generic buffer sizes. */
#define SMALL_BUF_SIZE			20
#define LARGE_BUF_SIZE 			100

#define AUDIO_BUF_SIZE     		128
#define WORD_COUNT   			(AUDIO_BUF_SIZE/2)

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048

OS_STK    GestureRecognitionTask_stk[TASK_STACKSIZE];
OS_STK    LCDTask_stk[TASK_STACKSIZE];
OS_STK    AudioTask_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */
#define AUDIO_TASK_PRIORITY					3
#define GESTURE_RECOGNITION_TASK_PRIORITY	1
#define LCD_TASK_PRIORITY					2

/* Screen Definitions */
#define UPPER_SCREEN             0
#define LOWER_SCREEN             1
#define M_POSITION               0
#define N_POSITION               8
#define INIT_POSITION            0
#define SCREEN_ROW_SIZE			16

/* Audio definitions */
#define PLAY 					20
#define PAUSE					21

#define VOLUME_STEPS			3
#define DEFAULT_VOLUME			50
#define MIN_VOLUME				0
#define MAX_VOLUME				100

/* Globals */
alt_up_character_lcd_dev * char_lcd_dev;

alt_up_audio_dev * audio_dev;
alt_up_av_config_dev* av_dev;
EmbeddedFileSystem efsl;
File readFile;

/* Queue for LCD to print message. */
#define LCDQSize 2
OS_EVENT *lcd_queue;
void *LCDQueue[LCDQSize];

/* Queue for position data. */
#define PositionQSize 2
OS_EVENT *position_queue;
void *PositionQueue[PositionQSize];

/* Semaphore for writing audio to queue */
OS_EVENT *audio_sem;

/* This function was borrowed from: Network-Controllable Embedded MP3 Player
https://www.ualberta.ca/~delliott/local/ece492/projects/2013w/g12_EmbeddedMP3/g12_EmbeddedMP3Player.zip */
static int audio_set_headphone_volume(alt_up_audio_dev * audio_codec, int volume) {
	int nVolume = (((((volume) * 80) / 100) + 48) - 1);
	int err = 0;
	err += alt_up_av_config_write_audio_cfg_register(audio_codec, AUDIO_REG_LEFT_HEADPHONE_OUT, nVolume);
	err += alt_up_av_config_write_audio_cfg_register(audio_codec, AUDIO_REG_RIGHT_HEADPHONE_OUT, nVolume);
	return err;
}

/* Simple utility function to ensure set volume isn't too high or low. */
static int boundNumber(int x, int upper, int lower) {
	if (x < lower) {
		return lower;
	} else if (x > upper) {
		return upper;
	} else {
		return x;
	}
}

/* Increase the volume for the audio component */
static int increaseVolume(int volume) {
	return boundNumber(volume + VOLUME_STEPS, MAX_VOLUME, MIN_VOLUME);
}

/* Decrease the volume for the audio component */
static int decreaseVolume(int volume) {
	return boundNumber(volume - VOLUME_STEPS, MAX_VOLUME, MIN_VOLUME);
}

/* Handle the commands for each gesture */
static void handleGestureCommands(int gesture_code) {
	INT8U err;

	static int state = PAUSE;
	static int volume = 70;

	char *message = (char *) calloc(LARGE_BUF_SIZE, sizeof(char));

	// Pause
	if ((state == PLAY) && (gesture_code == GCODE_DOWN_LEFT)) {
		OSTaskSuspend(AUDIO_TASK_PRIORITY);
		state = PAUSE;
		snprintf(message, LARGE_BUF_SIZE, "RECOGNIZED DOWN-LEFT: PAUSE");
	// Play
	} else if ((state == PAUSE) && (gesture_code == GCODE_UP_LEFT)) {
		OSTaskResume(AUDIO_TASK_PRIORITY);
		state = PLAY;
		snprintf(message, LARGE_BUF_SIZE, "RECOGNIZED UP-LEFT: PLAY");
	// Volume down
	} else if ((state == PLAY) && (gesture_code == GCODE_DOWN_RIGHT)) {
		volume = decreaseVolume(volume);
		audio_set_headphone_volume(av_dev, volume);
		snprintf(message, LARGE_BUF_SIZE, "RECOGNIZED DOWN-RIGHT: VOL-DOWN");
	// Volume up
	} else if ((state == PLAY) && (gesture_code == GCODE_UP_RIGHT)) {
		volume = increaseVolume(volume);
		audio_set_headphone_volume(av_dev, volume);
		snprintf(message, LARGE_BUF_SIZE, "RECOGNIZED UP-RIGHT: VOL-UP");
	}

	printf("%s\n", message);
	if ((err = OSQPost(lcd_queue, message)) != OS_NO_ERR) {
		printf("Error %d: message not put on LCD queue.\n", err);
	}
}

/* IRQ for the detector component indicating that centroid information
 * is ready.
 */
static void detector_irq_handler (void * context) {
	int centroid = IORD_32DIRECT(LED_DETECTOR_BASE, 0);
	OSQPost(position_queue, (void *) centroid);
}

/* The audio task is responsible for continuously playing music
 * off the SD card.
 */
void audio_task(void *pdata) {
	int err = 0;

	err += alt_up_av_config_write_audio_cfg_register(av_dev, AUDIO_REG_ACTIVE_CTRL, 0x00);
	err += alt_up_av_config_write_audio_cfg_register(av_dev, AUDIO_REG_LEFT_LINE_IN, 0x97);
	err += alt_up_av_config_write_audio_cfg_register(av_dev, AUDIO_REG_RIGHT_LINE_IN, 0x97);
	err += alt_up_av_config_write_audio_cfg_register(av_dev, AUDIO_REG_LEFT_HEADPHONE_OUT, 0x79);
	err += alt_up_av_config_write_audio_cfg_register(av_dev, AUDIO_REG_RIGHT_HEADPHONE_OUT, 0x79);
	err += alt_up_av_config_write_audio_cfg_register(av_dev, AUDIO_REG_ANALOG_AUDIO_PATH_CTRL, 0x12);
	err += alt_up_av_config_write_audio_cfg_register(av_dev, AUDIO_REG_DIGITAL_AUDIO_PATH_CTRL, 0x05);
	err += alt_up_av_config_write_audio_cfg_register(av_dev, AUDIO_REG_POWER_DOWN_CTRL, 0x07);
	err += alt_up_av_config_write_audio_cfg_register(av_dev, AUDIO_REG_AUDIO_DIGITAL_INTERFACE, 0x42);
	err += alt_up_av_config_write_audio_cfg_register(av_dev, AUDIO_REG_SAMPLING_CTRL, 0x22);
	err += alt_up_av_config_write_audio_cfg_register(av_dev, AUDIO_REG_ACTIVE_CTRL, 0x01);
	err += audio_set_headphone_volume(av_dev, DEFAULT_VOLUME);

	if(err < 0)
		printf("Audio Configuration Failed\n");

	alt_up_audio_reset_audio_core(audio_dev);
	alt_up_audio_disable_read_interrupt(audio_dev);
	alt_up_audio_disable_write_interrupt(audio_dev);

	char fileName[10] = {"class.wav\0"};
	if (file_fopen(&readFile, &efsl.myFs, fileName, 'r') != 0)
		printf("Error:\tCould not open file\n");

	int readSize = 0;
	euint32 currentSize = 44;

	euint8 buf[AUDIO_BUF_SIZE];
	int i;

	/* The task is suspended so that it can be played by another task. */
	OSTaskSuspend(AUDIO_TASK_PRIORITY);
	while(1) {
		if (currentSize < readFile.FileSize) {
			int fifospace = alt_up_audio_write_fifo_space(audio_dev, ALT_UP_AUDIO_LEFT);
			if (fifospace > WORD_COUNT) {
				readSize = file_fread(&readFile, currentSize, AUDIO_BUF_SIZE, buf);
				currentSize += readSize;
				i = 0;
				while(i < AUDIO_BUF_SIZE) {
					IOWR_ALT_UP_AUDIO_LEFTDATA(audio_dev->base, (buf[i+1]<<8)|buf[i]);
					IOWR_ALT_UP_AUDIO_RIGHTDATA(audio_dev->base, (buf[i+3]<<8)|buf[i+2]);

					i+=4;
				}
			}
		} else {
			currentSize = 44;
		}
	}

	file_fclose(&readFile);
}

/* This task is responsible for gesture recognition. */
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
    int mode = SEARCH_MODE;

	struct SearchNode *storage_base = getBase();
	printStorage(storage_base->next);

	struct DirectionNode *incoming_node, *last_incoming_node, *base;
    last_incoming_node = getDummyBase();

	while (1) {
		int centroid = (int) OSQPend(position_queue, 0, &err);
		if (err == OS_NO_ERR) {
			end_time = OSTimeGet();
			frame_count += 1;
			time_passed = (end_time - start_time) / OS_TICKS_PER_SEC;

			x = centroid & 1023;
			y = centroid >> 16;
			frame_rate = frame_count / time_passed;
;
			incoming_node = createDirectionNode(x, y, NO_GESTURE);

			/* We ignore position information that falls within the same grid */
			if (compareTwoDirectionNodes(incoming_node, last_incoming_node) == NODES_DIFFERENT) {

                if (mode == SEARCH_MODE) {
			        base = firstDirectionNode(incoming_node, last_incoming_node);
                } else {
				    base = nextDirectionNode(incoming_node, base, last_incoming_node, &direction_code);
                }

				if (base) {
					// Leaf node
					if (base->gesture_code != NO_GESTURE) {
                        mode = SEARCH_MODE;
                        handleGestureCommands(base->gesture_code);

					} else {
                        mode = FOLLOW_MODE;
					}
				} else {
                    mode = SEARCH_MODE;
				}

		        free(last_incoming_node);
		        last_incoming_node = incoming_node;

			} else {
			}
		} else {
			printf("Error receiving centroid!\n");
		}
	}
}

/* LCD_task is responsible for printing to the LED */
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
int main(void) {
	INT8U err;

	/* Initialize all the different components. */

	// open the Audio port
	av_dev = alt_up_av_config_open_dev(AV_CONFIG_NAME);
	if (av_dev == NULL)
		printf ("Error: could not open AV device \n");

	audio_dev = alt_up_audio_open_dev (AUDIO_0_NAME);
	if (audio_dev == NULL)
		printf("Error: could not open audio device \n");

	audio_sem = OSSemCreate(0);
	if (audio_sem == NULL)
		printf("Error: could not initialize semaphore \n");

	// Initialize efsl
	int ret = efs_init(&efsl, SPI_0_NAME);
	if(ret != 0)
		printf("...could not initialize filesystem.\n");

	lcd_queue = OSQCreate(LCDQueue, LCDQSize);
	if (lcd_queue == NULL)
		printf("Error: could not create LCD queue\n");

	position_queue = OSQCreate(PositionQueue, PositionQSize);
	if (position_queue == NULL)
		printf("Error: could not create position queue\n");

	err = alt_ic_isr_register(LED_DETECTOR_IRQ_INTERRUPT_CONTROLLER_ID,
			LED_DETECTOR_IRQ,
			detector_irq_handler,
			NULL,
			NULL);
	if (err != 0)
		printf("Error: could not initialize detector interrupt\n");

	err = OSTaskCreateExt(audio_task,
					  NULL,
					  (void *)&AudioTask_stk[TASK_STACKSIZE-1],
					  AUDIO_TASK_PRIORITY,
					  AUDIO_TASK_PRIORITY,
					  AudioTask_stk,
					  TASK_STACKSIZE,
					  NULL,
					  0);

	if (err != OS_NO_ERR)
		printf("Audio task not created.\n");

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
