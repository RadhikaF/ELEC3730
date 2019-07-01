#include "Ass-03.h"
#define BUTTON_NUM 8

//
// This task can be used as the main pulse rate application as it takes
// input from the front panel.
//
// *** MAKE UPDATES TO THE CODE AS REQUIRED ***
//
// Draw the boxes that make up the buttons and take action when the
// buttons are pressed. See suggested updates for the touch panel task
// that checks for button presses. Can do more in that task.

void Ass_03_Task_02(void const * argument)
{
	uint32_t loop=0;

	Coordinate display;

	osSignalWait(1,osWaitForever);
	safe_printf("Hello from Task 2 - Pulse Rate Application (touch screen input)\n");

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
	int i = 0;
	int result = 0;

	while (1)
	{
		if (getfp(&display) == 0)
		{
			//safe_printf("Task 2: %d (got  %3d,%3d)\n", loop, display.x, display.y);
			for (i = 0; i < BUTTON_NUM; i++)
			{
				//safe_printf("\ty1 = %d, y2 = %d,\nx1 = %d, x2 = %d,\nscreen position y = %d, screen position x = %d\n",
				//		screen_list[i].ypos1, screen_list[i].ypos2, screen_list[i].xpos1, screen_list[i].xpos2, display.y, display.x);
				if((display.y > screen_list[i].ypos1) && (display.y < screen_list[i].ypos2) &&
						(display.x > screen_list[i].xpos1) && (display.x < screen_list[i].xpos2))
				{
					safe_printf("Task 2: %d (got  %3d,%3d)\n", loop, display.x, display.y);
					result = screen_list[i].Function();
					break;
				}
			}
		}
	}
}

int play()
{
	//play
	safe_printf("play\n");
	set_stop(0);
	safe_printf("stop state: %d\n", state.stop);
	return 0;
}
int stop()
{
	//stop
	safe_printf("stop\n");
	set_stop(1);
	safe_printf("stop state: %d\n", state.stop);
	return 0;
}
int save()
{
	//save
	safe_printf("save\n");
	return 0;
}
int load()
{
	//load
	safe_printf("load\n");
	return 0;
}
int debug()
{
	//func1
	safe_printf("debug\n");
	/*
	int debug_state = get_debug();
	if (debug_state == 1)
	{
		safe_printf("Debug is currently on\n");
	}
	else
	{
		safe_printf("Debug is currently off\n");
	}*/
	return 0;
}
int mem1()
{
	//mem1
	safe_printf("mem1\n");
	return 0;
}
int mem2()
{
	//mem2
	safe_printf("mem2\n");
	return 0;
}
int mem3()
{
	//mem3
	safe_printf("mem3\n");
	return 0;
}
