/*
 * debug.c
 *
 *  Created on: 23 May 2018
 *      Author: Joshua Beverley (c3195884)
 */

#include "Ass-03.h"
#include "debug.h"

extern void info_init()
{
	osDelay(500);
	osMutexWait(myMutex04Handle, osWaitForever);
	info.debug_variable = 0;
	info.stop_variable = 0;
	info.speed_variable = 0;
	osMutexRelease(myMutex04Handle);
}

extern void screen_init()
{
	// Display welcome message
	osMutexWait(myMutex01Handle, osWaitForever);
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DrawRect(5, 5, 58, 52);
	BSP_LCD_DrawRect(68, 5, 58, 52);
	BSP_LCD_DrawRect(131, 5, 58, 52);
	BSP_LCD_DrawRect(194, 5, 58, 52);
	BSP_LCD_DrawRect(257, 5, 58, 52);
	BSP_LCD_DrawRect(5, 62, 58, 52);
	BSP_LCD_DrawRect(5, 119, 58, 52);
	BSP_LCD_DrawRect(5, 176, 58, 52);
	BSP_LCD_DisplayStringAt(32, 22, (uint8_t*) "Load", CENTER_MODE);
	BSP_LCD_DisplayStringAt(286, 22, (uint8_t*) "Debug", CENTER_MODE);
	BSP_LCD_DisplayStringAt(32, 79, (uint8_t*) "Mem1", CENTER_MODE);
	BSP_LCD_DisplayStringAt(32, 136, (uint8_t*) "Mem2", CENTER_MODE);
	BSP_LCD_DisplayStringAt(32, 193, (uint8_t*) "Mem3", CENTER_MODE);
	drawplay(149, 20);
	drawstop(212, 20);
	drawsave(97, 31);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font8);
	BSP_LCD_DisplayStringAt(160, 230, (uint8_t*) "JOSHUA BEVERLEY & RADHIKA FERON ELEC3730 ASSIGNMENT 3", CENTER_MODE);
	BSP_LCD_SetFont(&Font16);
	osMutexRelease(myMutex01Handle);
}

void drawplay(int xpos, int ypos)
{
	// xpos, ypos top corner of play button
	int array_nos[21] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 19, 17, 15, 13, 11, 9, 7, 5, 3, 1};
	int new_y_pos = ypos;
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	for (int i = 0; i < 21; i++)
	{
		new_y_pos = ypos + i;
		BSP_LCD_DrawHLine(xpos, new_y_pos, array_nos[i]);
	}

}
void drawstop(int xpos, int ypos)
{
	// xpos, ypos top corner of play button
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_FillRect(xpos, ypos, 20, 20);		// xpos, ypos top corner of stop button
}
void drawsave(int xpos, int ypos)
{
	// xpos, ypos centre of save button
	BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
	BSP_LCD_FillRect(xpos-14, ypos-13, 28, 28);
	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
	BSP_LCD_FillRect(xpos-9, ypos-14, 18, 14);
	BSP_LCD_FillRect(xpos-7, ypos+5, 14, 9);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DrawHLine(xpos-5, ypos+8, 10);
	BSP_LCD_DrawHLine(xpos-5, ypos+12, 10);
}

extern int get_debug()
{
	osMutexWait(myMutex04Handle, osWaitForever);
	int temp = info.debug_variable;
	osMutexRelease(myMutex04Handle);
	return temp;
}

extern void set_debug(int var)
{
	osMutexWait(myMutex04Handle, osWaitForever);
	info.debug_variable = var;
	osMutexRelease(myMutex04Handle);
}

extern int get_stop()
{
	osMutexWait(myMutex04Handle, osWaitForever);
	int temp = info.stop_variable;
	osMutexRelease(myMutex04Handle);
	return temp;
}

extern void set_stop(int stop)
{
	osMutexWait(myMutex04Handle, osWaitForever);
	info.stop_variable = stop;
	osMutexRelease(myMutex04Handle);
}

