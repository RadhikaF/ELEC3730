/*
 * drawLCD.c
 *
 *  Created on: 7Jun.,2018
 *      Author: c3195884 & c3256870
 */

#include "Ass-03.h"
#define XOFF 69		// where the left edge of the display is, in LCD screen pixels
#define YOFF 63		// where the top edge of the display is, in LCD screen pixels
#define XSIZE 245	// the length of the x direction of the screen (left to right) in LCD pixels
#define YSIZE 152	// the length of the y direction of the screen (top to bottom) in LCD pixels
#define MAX_POINTS 6		// maximum number of axis lines

void screen_init()
{
	// Display welcome message
	osMutexWait(myMutex01Handle, osWaitForever);
	BSP_LCD_Clear(LCD_COLOR_WHITE);		// clear LCD screen
	BSP_LCD_SetFont(&Font24);		// set font size to 24
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);		// set text to black
	BSP_LCD_DisplayStringAt(160, 14, (uint8_t*) "Joshua", CENTER_MODE);		// print opening message
	BSP_LCD_DisplayStringAt(160, 40, (uint8_t*) "Beverley", CENTER_MODE);
	BSP_LCD_DisplayStringAt(160, 66, (uint8_t*) "c3195884", CENTER_MODE);
	BSP_LCD_DisplayStringAt(160, 92, (uint8_t*) "Radhika", CENTER_MODE);
	BSP_LCD_DisplayStringAt(160, 118, (uint8_t*) "Feron", CENTER_MODE);
	BSP_LCD_DisplayStringAt(160, 143, (uint8_t*) "c3256870", CENTER_MODE);
	BSP_LCD_DisplayStringAt(160, 170, (uint8_t*) "ELEC3730", CENTER_MODE);
	BSP_LCD_DisplayStringAt(160, 196, (uint8_t*) "Assignment", CENTER_MODE);
	osDelay(500);		// 1000ms delay
	BSP_LCD_Clear(LCD_COLOR_WHITE);		// clear screen again
	BSP_LCD_SetFont(&Font16);		// set font size to 16
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DrawRect(5, 5, 58, 52);		// draw the buttons on the screen
	BSP_LCD_DrawRect(68, 5, 58, 52);
	BSP_LCD_DrawRect(131, 5, 58, 52);
	BSP_LCD_DrawRect(194, 5, 58, 52);
	BSP_LCD_DrawRect(257, 5, 58, 52);
	BSP_LCD_DrawRect(5, 62, 58, 52);
	BSP_LCD_DrawRect(5, 119, 58, 52);
	BSP_LCD_DrawRect(5, 176, 58, 52);
	//BSP_LCD_DisplayStringAt(32, 22, (uint8_t*) "Load", CENTER_MODE);		// write names for buttons
	//BSP_LCD_DisplayStringAt(286, 22, (uint8_t*) "Erase", CENTER_MODE);
	//BSP_LCD_DisplayStringAt(32, 79, (uint8_t*) "Mem1", CENTER_MODE);
	//BSP_LCD_DisplayStringAt(32, 136, (uint8_t*) "+", CENTER_MODE);
	//BSP_LCD_DisplayStringAt(32, 193, (uint8_t*) "-", CENTER_MODE);
	BSP_LCD_SetFont(&Font8);
	drawplay(149, 17);		// sketch the load, save, stop and play buttons in these functions
	drawstop(212, 17);
	drawsave(97, 28);
	drawload(34, 28);
	drawerase(286, 28);
	drawmem(34, 85);
	drawspeedincrease(34, 142);
	drawspeeddecrease(34, 199);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font8);
	BSP_LCD_DisplayStringAt(160, 230, (uint8_t*) "JOSHUA BEVERLEY & RADHIKA FERON ELEC3730 ASSIGNMENT 3", CENTER_MODE);		// display Joshua and Radhika at the bottom of the screen
	BSP_LCD_SetFont(&Font16);
	osMutexRelease(myMutex01Handle);
}

void drawaxis(int speed)	// this function draws the x axis for the pulse graph, with numbers and vertical lines
{
	if (speed >= 1 && speed <= 50)		// double checking the speed, function shouldn't run if this condition isn't met
	{
		char axis_string[MAX_POINTS][5];		// initialise both the string holding the numbers as strings
		int axis_values[MAX_POINTS] = {0};		// and as integers
		BSP_LCD_SetTextColor(LCD_COLOR_WHITE);			// white text colout
		BSP_LCD_FillRect(XOFF-3, YOFF+YSIZE+1, XSIZE+6, 10);	// draw a white box at the axis to clear any old axises
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);			// black text colour
		BSP_LCD_SetFont(&Font8);		// size 8 text
		BSP_LCD_DrawVLine(XOFF, YOFF+YSIZE+1, 3);		// draw vertical line at selected position (in this case at the start)
		axis_values[1] = 0;			// set the integer value to a fraction of speed (in this case 0 for the start of the axis)
		sprintf(axis_string[1], "%d", axis_values[1]);		// convert string to integer
		BSP_LCD_DisplayStringAt(XOFF + 1, YOFF+YSIZE+5, (uint8_t*) axis_string[1], CENTER_MODE);		// display the string on the LCD screen below the vertical line
		BSP_LCD_DrawVLine(XOFF + (XSIZE - 1), YOFF + YSIZE + 1, 3);			// last 4 steps repeated
		axis_values[6] = speed;
		sprintf(axis_string[6], "%d", axis_values[6]);
		BSP_LCD_DisplayStringAt(XOFF + (XSIZE - 1) - 2, YOFF+YSIZE+5, (uint8_t*) axis_string[6], CENTER_MODE);
		if (get_debug())
		{
			printf("%saxis values as strings: %s, %s\naxis values as integers: %d, %d\n%s", GREEN, axis_string[1], axis_string[6], axis_values[1], axis_values[6], WHITE);		// print to putty the successful axis change
		}
		if ((speed % 5) == 0)		// if else statement checking if speed is a multiple of 5, 4, 3 or 2
		{
			// if a multiple of 5, 6 axis lines drawn following the same 4 steps as above
			BSP_LCD_DrawVLine(XOFF + (XSIZE - 1) / 5, YOFF+YSIZE+1, 3);
			axis_values[2] = speed / 5;
			sprintf(axis_string[2], "%d", axis_values[2]);
			BSP_LCD_DisplayStringAt(XOFF + (XSIZE - 1) / 5, YOFF+YSIZE+5, (uint8_t*) axis_string[2], CENTER_MODE);
			BSP_LCD_DrawVLine(XOFF + 2 * (XSIZE - 1) / 5, YOFF + YSIZE + 1, 3);
			axis_values[3] = 2 * speed / 5;
			sprintf(axis_string[3], "%d", axis_values[3]);
			BSP_LCD_DisplayStringAt(XOFF + 2 * (XSIZE - 1) / 5, YOFF+YSIZE+5, (uint8_t*) axis_string[3], CENTER_MODE);
			BSP_LCD_DrawVLine(XOFF + 3 * (XSIZE - 1) / 5, YOFF + YSIZE + 1, 3);
			axis_values[4] = 3 * speed / 5;
			sprintf(axis_string[4], "%d", axis_values[4]);
			BSP_LCD_DisplayStringAt(XOFF + 3 * (XSIZE - 1) / 5, YOFF+YSIZE+5, (uint8_t*) axis_string[4], CENTER_MODE);
			BSP_LCD_DrawVLine(XOFF + 4 * (XSIZE - 1) / 5, YOFF + YSIZE + 1, 3);
			axis_values[5] = 4 * speed / 5;
			sprintf(axis_string[5], "%d", axis_values[5]);
			BSP_LCD_DisplayStringAt(XOFF + 4 * (XSIZE - 1) / 5, YOFF+YSIZE+5, (uint8_t*) axis_string[5], CENTER_MODE);
		}
		else if ((speed % 4) == 0)
		{
			// if a multiple of 4, 5 axis lines drawn following the same 4 steps as above
			BSP_LCD_DrawVLine(XOFF + (XSIZE - 1) / 4, YOFF+YSIZE+1, 3);
			axis_values[2] = speed / 4;
			sprintf(axis_string[2], "%d", axis_values[2]);
			BSP_LCD_DisplayStringAt(XOFF + (XSIZE - 1) / 4, YOFF+YSIZE+5, (uint8_t*) axis_string[2], CENTER_MODE);
			BSP_LCD_DrawVLine(XOFF + 2 * (XSIZE - 1) / 4, YOFF + YSIZE + 1, 3);
			axis_values[3] = 2 * speed / 4;
			sprintf(axis_string[3], "%d", axis_values[3]);
			BSP_LCD_DisplayStringAt(XOFF + 2 * (XSIZE - 1) / 4, YOFF+YSIZE+5, (uint8_t*) axis_string[3], CENTER_MODE);
			BSP_LCD_DrawVLine(XOFF + 3 * (XSIZE - 1) / 4, YOFF + YSIZE + 1, 3);
			axis_values[4] = 3 * speed / 4;
			sprintf(axis_string[4], "%d", axis_values[4]);
			BSP_LCD_DisplayStringAt(XOFF + 3 * (XSIZE - 1) / 4, YOFF+YSIZE+5, (uint8_t*) axis_string[4], CENTER_MODE);
		}
		else if ((speed % 3) == 0)
		{
			// if a multiple of 3, 4 axis lines drawn following the same 4 steps as above
			BSP_LCD_DrawVLine(XOFF + (XSIZE - 1) / 3, YOFF+YSIZE+1, 3);
			axis_values[2] = speed / 3;
			sprintf(axis_string[2], "%d", axis_values[2]);
			BSP_LCD_DisplayStringAt(XOFF + (XSIZE - 1) / 3, YOFF+YSIZE+5, (uint8_t*) axis_string[2], CENTER_MODE);
			BSP_LCD_DrawVLine(XOFF + 2 * (XSIZE - 1) / 3, YOFF + YSIZE + 1, 3);
			axis_values[3] = 2 * speed / 3;
			sprintf(axis_string[3], "%d", axis_values[3]);
			BSP_LCD_DisplayStringAt(XOFF + 2 * (XSIZE - 1) / 3, YOFF+YSIZE+5, (uint8_t*) axis_string[3], CENTER_MODE);
		}
		else if ((speed % 2) == 0)
		{
			// if a multiple of 2, 3 axis lines drawn following the same 4 steps as above
			BSP_LCD_DrawVLine(XOFF + (XSIZE - 1) / 2, YOFF+YSIZE+1, 3);
			axis_values[2] = speed / 2;
			sprintf(axis_string[2], "%d", axis_values[2]);
			BSP_LCD_DisplayStringAt(XOFF + (XSIZE - 1) / 2, YOFF+YSIZE+5, (uint8_t*) axis_string[2], CENTER_MODE);
		}
		BSP_LCD_SetFont(&Font16);		// reset font size to 16
	}
	else		// if these conditions not met, print error on putty and return with no change to axis
	{
		if (get_debug())
		{
			safe_printf("%sError: Speed input is out of range%s\n", RED, WHITE);
		}
	}
}

void drawplay(int xpos, int ypos)
{
	// xpos, ypos top corner of play button
	int array_nos[21] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 19, 17, 15, 13, 11, 9, 7, 5, 3, 1};		// constants for play button sketch
	int new_y_pos = ypos;
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);	// green colour selected
	for (int i = 0; i < 21; i++)		// for loop incrementing up the y axis
	{
		new_y_pos = ypos + i;
		BSP_LCD_DrawHLine(xpos, new_y_pos, array_nos[i]);	// horizontal lines drawn in play button shape
	}
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(xpos+11, ypos+30, (uint8_t*) "PLAY", CENTER_MODE);
}
void drawstop(int xpos, int ypos)
{
	// xpos, ypos top corner of stop button
	BSP_LCD_SetTextColor(LCD_COLOR_RED);		// literally a red box
	BSP_LCD_FillRect(xpos, ypos, 20, 20);		// xpos, ypos top corner of stop button
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(xpos+11, ypos+30, (uint8_t*) "STOP", CENTER_MODE);
}
void drawsave(int xpos, int ypos)
{
	// xpos, ypos centre of save button
	BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);		// orange and light grey colours used to draw save button
	BSP_LCD_FillRect(xpos-14, ypos-13, 28, 27);		// design includes 3 rectangles (1 orange 2 grey) and 2 lines drawn on the lower grey box
	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
	BSP_LCD_FillRect(xpos-9, ypos-14, 18, 14);
	BSP_LCD_FillRect(xpos-7, ypos+5, 14, 8);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DrawHLine(xpos-5, ypos+7, 10);
	BSP_LCD_DrawHLine(xpos-5, ypos+11, 10);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(xpos, ypos+19, (uint8_t*) "SAVE", CENTER_MODE);
}

void drawload(int xpos, int ypos)
{
	// xpos, ypos centre of load button
	int array_nos1[25] = {4, 8, 12, 16, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 20, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22};
	int array_nos2[28] = {18, 19, 20, 28, 28, 28, 28, 27, 26, 25, 24, 23, 22, 14, 14, 13, 12, 12, 12, 11, 10, 10, 10, 10, 10, 10, 10, 10};
	for (int i = 0; i < 28; i++)
	{
		BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
		BSP_LCD_DrawHLine(xpos-17 + (i/4), ypos-14+i, array_nos2[i]);
		if (i < 5)
		{
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_DrawHLine(xpos-17+ (i/4), ypos-14+i, 4-i);
		}
	}
	BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);		// orange and light grey colours used to draw save button
	for (int i = 0; i < 25; i++)
	{
		BSP_LCD_DrawLine(xpos-11 + i, ypos+14, xpos-10 + i, ypos+14 - (array_nos1[i]));
	}
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(xpos, ypos+19, (uint8_t*) "LOAD", CENTER_MODE);
}

void drawerase(int xpos, int ypos)
{
	// xpos, ypos centre of erase button
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DrawHLine(xpos-17, ypos+14, 20);
	BSP_LCD_DrawVLine(xpos-17, ypos-4, 18);
	BSP_LCD_DrawHLine(xpos-17, ypos+13, 20);
	BSP_LCD_DrawVLine(xpos-16, ypos-4, 18);
	BSP_LCD_DrawLine(xpos-14, ypos+11, xpos-11, ypos);
	BSP_LCD_DrawLine(xpos-11, ypos, xpos-6, ypos+8);
	BSP_LCD_DrawLine(xpos-6, ypos+8, xpos-3, ypos+3);
	BSP_LCD_DrawLine(xpos-3, ypos+3, xpos+1, ypos+11);
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_DrawLine(xpos-2, ypos+2, xpos+14, ypos-14);
	BSP_LCD_DrawLine(xpos-2, ypos-14, xpos+14, ypos+2);
	BSP_LCD_DrawLine(xpos-3, ypos+2, xpos+13, ypos-14);
	BSP_LCD_DrawLine(xpos-3, ypos-14, xpos+13, ypos+2);
	BSP_LCD_DrawLine(xpos-4, ypos+2, xpos+12, ypos-14);
	BSP_LCD_DrawLine(xpos-4, ypos-14, xpos+12, ypos+2);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DisplayStringAt(xpos, ypos+19, (uint8_t*) "ERASE", CENTER_MODE);
}

void drawmem(int xpos, int ypos)
{
	// xpos, ypos centre of memory button
	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
	BSP_LCD_DrawRect(xpos-17, ypos-14, 20, 28);
	BSP_LCD_DrawRect(xpos-16, ypos-13, 18, 26);
	BSP_LCD_DrawRect(xpos-15, ypos-12, 16, 24);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(xpos-1, ypos-14, 5, 4);
	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTBLUE);
	BSP_LCD_DrawLine(xpos-1, ypos-14, xpos+3, ypos-10);
	BSP_LCD_DrawLine(xpos-1, ypos-13, xpos+2, ypos-10);
	BSP_LCD_DrawLine(xpos-1, ypos-12, xpos+1, ypos-10);
	BSP_LCD_FillRect(xpos+1, ypos+11, 3, 3);
	BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);
	BSP_LCD_DrawHLine(xpos-13, ypos-7, 13);
	BSP_LCD_DrawHLine(xpos-13, ypos-3, 13);
	BSP_LCD_DrawHLine(xpos-13, ypos+1, 13);
	BSP_LCD_DrawHLine(xpos-13, ypos+5, 13);
	BSP_LCD_DrawHLine(xpos-13, ypos+9, 13);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAt(50, 79, (uint8_t*) "1", CENTER_MODE);
	BSP_LCD_SetFont(&Font8);
	BSP_LCD_DisplayStringAt(xpos, ypos+19, (uint8_t*) "MEMORY", CENTER_MODE);
}

void drawspeedincrease(int xpos, int ypos)
{
	// xpos, ypos centre of speed button
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DrawCircle(xpos, ypos+4, 18);
	BSP_LCD_DrawCircle(xpos, ypos+4, 17);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(xpos-19, ypos+4, 38, 19);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DrawHLine(xpos-18, ypos+14, 36);
	BSP_LCD_DrawHLine(xpos-18, ypos+13, 36);
	BSP_LCD_DrawVLine(xpos-18, ypos+4, 10);
	BSP_LCD_DrawVLine(xpos+18, ypos+4, 10);
	BSP_LCD_DrawVLine(xpos-17, ypos+4, 10);
	BSP_LCD_DrawVLine(xpos+17, ypos+4, 10);
	BSP_LCD_FillCircle(xpos, ypos+5, 2);
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_DrawLine(xpos+2, ypos+3, xpos+11, ypos-6);
	BSP_LCD_DrawLine(xpos+3, ypos+3, xpos+11, ypos-5);
	BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA);
	BSP_LCD_DrawHLine(xpos-5, ypos-4, 9);
	BSP_LCD_DrawLine(xpos+2, ypos-6, xpos+4, ypos-4);
	BSP_LCD_DrawLine(xpos+2, ypos-2, xpos+4, ypos-4);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font8);
	BSP_LCD_DisplayStringAt(xpos, ypos+19, (uint8_t*) "FASTER", CENTER_MODE);
}

void drawspeeddecrease(int xpos, int ypos)
{
	// xpos, ypos centre of speed button
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DrawCircle(xpos, ypos+4, 18);
	BSP_LCD_DrawCircle(xpos, ypos+4, 17);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(xpos-19, ypos+4, 38, 19);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DrawHLine(xpos-18, ypos+14, 36);
	BSP_LCD_DrawHLine(xpos-18, ypos+13, 36);
	BSP_LCD_DrawVLine(xpos-18, ypos+4, 10);
	BSP_LCD_DrawVLine(xpos+18, ypos+4, 10);
	BSP_LCD_DrawVLine(xpos-17, ypos+4, 10);
	BSP_LCD_DrawVLine(xpos+17, ypos+4, 10);
	BSP_LCD_FillCircle(xpos, ypos+5, 2);
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_DrawLine(xpos-2, ypos+3, xpos-11, ypos-6);
	BSP_LCD_DrawLine(xpos-3, ypos+3, xpos-11, ypos-5);
	BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA);
	BSP_LCD_DrawHLine(xpos-4, ypos-4, 9);
	BSP_LCD_DrawLine(xpos-2, ypos-6, xpos-4, ypos-4);
	BSP_LCD_DrawLine(xpos-2, ypos-2, xpos-4, ypos-4);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font8);
	BSP_LCD_DisplayStringAt(xpos, ypos+19, (uint8_t*) "SLOWER", CENTER_MODE);
}

void wipe_screen()		// this function clears the graph display screen, based on constants at top of screen
{
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(XOFF,YOFF,XSIZE-1,YSIZE-1);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
}
