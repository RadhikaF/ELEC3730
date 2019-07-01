/*
 * button.c
 *
 *  Created on: 6Jun.,2018
 *      Author: c3195884 & c3256870
 */

#include "Ass-03.h"
#define BUTTON_NUM 8	// number of buttons on screen

const structdata screen_list[] =
{
		{5, 63, 5, 57, 1, &load},	// structure containing x and y hitbox boundaries, the button number, and the function that runs when the button is pressed
		{68, 126, 5, 57, 2, &save},
		{131, 189, 5, 57, 3, &play},
		{194, 262, 5, 57, 4, &stop},
		{257, 315, 5, 57, 5, &erase},
		{5, 60, 62, 114, 6, &memory},
		{5, 60, 119, 171, 7, &more_speed},
		{5, 60, 176, 228, 8, &less_speed}
};
static int previous_button = 0;		// last button pressed
static int loop = 0;		// counts how many times a button is pressed on the screen

int button_select(int y_disp, int x_disp, int prev_button)
{
	int i = 0;		// loop variable
	int result = 0;		// result button pressed, always the program setting (button number) of the button pressed
	int success = 0;	// result from the function, always 0
	//safe_printf("Task 2: %d (got  %3d,%3d)\n", loop, display.x, display.y);
	for (i = 0; i < BUTTON_NUM; i++)		// cycle through all the buttons
	{
		//safe_printf("\ty1 = %d, y2 = %d,\nx1 = %d, x2 = %d,\nscreen position y = %d, screen position x = %d\n",
		//		screen_list[i].ypos1, screen_list[i].ypos2, screen_list[i].xpos1, screen_list[i].xpos2, display.y, display.x);
		if((y_disp > screen_list[i].ypos1) && (y_disp < screen_list[i].ypos2) &&
				(x_disp > screen_list[i].xpos1) && (x_disp < screen_list[i].xpos2))		// if button is pressed within the button hitbox
		{
			loop++;		// increment loop counter
			if (get_debug())	// if debug is on, output message
			{
				safe_printf("Task 2: %d (got  %3d,%3d)\n", loop, x_disp, y_disp);
			}
			osDelay(100);		// delay of 100ms
			result = screen_list[i].program_setting;	// update the result value with the button pressed
			if ((memory_save_state() == 0) && result == 1)		// if load is pressed and there is no memory saved
			{
				if (get_debug())
				{
					safe_printf("%sError: Cannot load an empty memory slot%s\n", RED, WHITE);	// say there's an error
				}
				wipe_screen();
				result = prev_button;	// reset the result to previous button as if nothing happened
			}
			else if (((result == 7) && (get_stop() == 1)) || ((result == 8) && (get_stop() == 1)))
			{
				if (get_debug())
				{
					safe_printf("%sError: Cannot change speed while graphing has stopped%s\n", RED, WHITE);	// say there's an error
				}
				result = prev_button;	// reset the result to previous button as if nothing happened
			}
			else
			{
				success = screen_list[i].Function();	// else run the function relating to the button pressed
			}
			break;
		}
		else
		{
			result = 0;		// if screen is pressed but no button pressed, return 0
		}
	}
	previous_button = result;
	return result;
}

int stop_function()		// used to reset screen coordinates in task 4 if the last button pressed requires it
{
	if ((previous_button == 5) || (previous_button == 1))		// if previous button pressed was erase
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
