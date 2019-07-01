/*
 * button.c
 *
 *  Created on: 6Jun.,2018
 *      Author: c3195884
 */

#include "Ass-03.h"
#define BUTTON_NUM 8

const structdata screen_list[] =
{
		{5, 63, 5, 57, 1, &load},
		{68, 126, 5, 57, 2, &save},
		{131, 189, 5, 57, 3, &play},		// structure containing char added to answer string, x and y hitbox boundaries, and the buttons "operation"
		{194, 262, 5, 57, 4, &stop},
		{257, 315, 5, 57, 5, &debug},
		{5, 60, 62, 114, 6, &mem1},
		{5, 60, 119, 171, 7, &mem2},
		{5, 60, 176, 228, 8, &mem3}
};

int button_select(int loop, int y_disp, int x_disp)
{
	int i = 0;
	int result = 0;
	int success = 0;
	//safe_printf("Task 2: %d (got  %3d,%3d)\n", loop, display.x, display.y);
	for (i = 0; i < BUTTON_NUM; i++)
	{
		//safe_printf("\ty1 = %d, y2 = %d,\nx1 = %d, x2 = %d,\nscreen position y = %d, screen position x = %d\n",
		//		screen_list[i].ypos1, screen_list[i].ypos2, screen_list[i].xpos1, screen_list[i].xpos2, display.y, display.x);
		if((y_disp > screen_list[i].ypos1) && (y_disp < screen_list[i].ypos2) &&
				(x_disp > screen_list[i].xpos1) && (x_disp < screen_list[i].xpos2))
		{
			safe_printf("Task 2: %d (got  %3d,%3d)\n", loop, x_disp, y_disp);
			osDelay(100);
			success = screen_list[i].Function();
			result = screen_list[i].program_setting;
			break;
		}
		else
		{
			result = 0;
		}
	}
	return result;
}
