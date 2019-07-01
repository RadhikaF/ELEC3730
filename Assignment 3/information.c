/*
 * information.c
 *
 *  Created on: 23 May 2018
 *      Author: Joshua Beverley (c3195884), Radhika Feron (c3256870)
 */

#include "information.h"
#define XSIZE 245
#define MEM_NO 5

void info_init()		// this function initialises the info variables at the start of the program running
{
	osMutexWait(myMutex04Handle, osWaitForever);	// open function specific mutex
	int result = 0;
	int i = 0;
	int j = 0;
	info.debug_variable = 1;		// set debug variable to 1 (debug initially on)
	info.stop_variable = 0;			// set stop variable to 0 (graph initially playing)
	info.speed_variable = 10;		// set speed to 10sec
	info.memory_number = 1;			// set memory to 1, displaying mem1 on screen
	info.temp_array[XSIZE+1];
	info.array_of_points[MEM_NO][XSIZE+1];
	info.memory_saved[MEM_NO];		// set all points of memory_saved to 0
	for (i = 0; i <= XSIZE; i++)		// initialise all values in temp array and array of points to 0
	{
		info.temp_array[i] = -1;
		for (j = 0; j < MEM_NO; j++)
		{
			info.array_of_points[j][i] = -1;
		}
	}
	for (i = 0; i < MEM_NO; i++)
	{
		info.memory_saved[i] = 0;
		//result = myReadFile();
		if (result == 1)
		{
			info.memory_saved[i] = 1;
		}
	}
	osMutexRelease(myMutex04Handle);		// close mutex 4
	set_temp_mem_value(10, XSIZE-1);
}

int get_debug()		// gets debug variable
{
	osMutexWait(myMutex04Handle, osWaitForever);	// mutex 4 on
	int temp = info.debug_variable;		// temporarily stores debug
	osMutexRelease(myMutex04Handle);	// mutex 4 off
	return temp;		// returns debug
}

void set_debug(int var)		// set debug to input variable
{
	osMutexWait(myMutex04Handle, osWaitForever);	// mutex 4 on
	info.debug_variable = var;		// set debug variable to input
	osMutexRelease(myMutex04Handle);	// mutex 4 off
}

int get_stop()		// gets stop variable
{
	osMutexWait(myMutex04Handle, osWaitForever);	// mutex 4 on
	int temp = info.stop_variable;		// temporarily stores stop
	osMutexRelease(myMutex04Handle);	// mutex 4 off
	return temp;		// return stop
}

void set_stop(int stop)		// set stop to input
{
	osMutexWait(myMutex04Handle, osWaitForever);	// mutex 4 on
	osDelay(50);
	info.stop_variable = stop;		// set stop variable to input
	osMutexRelease(myMutex04Handle);	// mutex 4 off
}

void change_memory(int memory_input)		// change the memory shown on screen as well as the one selected
{
	osMutexWait(myMutex04Handle, osWaitForever);	// mutex 1 on
	int temp;		// create temporary variable
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(45, 70, 10, 20);		// erase what is inside the memory button displayed on screen
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	char memory_string[5] = "";
	if (memory_input == 0)
	{
		for (int i = 1; i <= MEM_NO; i++)
		{
			if (info.memory_number == i)
			{
				if (info.memory_number == MEM_NO)
				{
					info.memory_number = 1;
				}
				else
				{
					info.memory_number++;
				}
				sprintf(memory_string, "%d", info.memory_number);
				BSP_LCD_SetFont(&Font24);
				BSP_LCD_DisplayStringAt(50, 79, (uint8_t*) memory_string, CENTER_MODE);
				BSP_LCD_SetFont(&Font16);
				break;
			}
		}
	}
	else
	{
		info.memory_number = memory_input;
		sprintf(memory_string, "Mem%d", info.memory_number);
		BSP_LCD_DisplayStringAt(32, 79, (uint8_t*) memory_string, CENTER_MODE);
	}
	temp = info.memory_number;	// temporarily store memory number
	osMutexRelease(myMutex04Handle);	// mutex 1 off
	if (get_debug())
	{
		safe_printf("%sMemory %d selected\n%s", CYAN, temp, WHITE);
		for (int i = 0; i < MEM_NO; i++)
		{
			safe_printf("%smemory state %d (selected = %d) = %d\n%s", YELLOW, i+1, info.memory_number, info.memory_saved[i], WHITE);
		}
	}
}

char* get_memory()			// get memory value as a string
{
	osMutexWait(myMutex04Handle, osWaitForever);		// mutex 4 on
	int temp = info.memory_number;						// store memory variable in temp
	static char destination_string[12] = "";			// create string
	sprintf(destination_string, "Memory%d.csv", temp);					// convert memory number from integer to string
	osMutexRelease(myMutex04Handle);					// mutex 4 off
	return destination_string;							// return string
}

int get_array_points_value(int position)		// gets values stored in memory on board
{
	osMutexWait(myMutex04Handle, osWaitForever);	// mutex 4 on
	int memory_no = info.memory_number;		// get memory number as integer
	int temp = info.array_of_points[memory_no-1][position];
	osMutexRelease(myMutex04Handle);	// mutex 4 off
	return temp;
}

void set_temp_mem_value(int value, int position)		// sets values from graph to temporary memory as it is graphing
{
	osMutexWait(myMutex04Handle, osWaitForever);	// mutex 4 on
	info.temp_array[position] = value;		// store input value in temporary array
	//	printf("set_mem_value: Array value [%d]: %d\n", position, info.temp_array[position]);
	osMutexRelease(myMutex04Handle);	// mutex 4 off
}

void temp_to_memory()		// transfers temporary memory array to the array of points, presumably before being saved to SD card
{
	osMutexWait(myMutex04Handle, osWaitForever);	// mutex 4 on
	int memory_no = info.memory_number;		// get memory number as integer
	for (int i = 0; i < XSIZE; i++)		// for the size of array
	{
		info.array_of_points[memory_no-1][i] = info.temp_array[i];	// save temp value to array of points number
		info.temp_array[i] = -1;		// sets temp array to 0
	}
	info.memory_saved[memory_no-1] = 1;		// memory saved set to 1, indicating there is memory saved in this memory slot
	osMutexRelease(myMutex04Handle);	// mutex 4 off
}

int memory_save_state()		// called from button.c, used to check if memory has been saved on SD card
{
	osMutexWait(myMutex04Handle, osWaitForever);	// mutex 4 on
	int memory_no = info.memory_number;		// get memory number as an integer
	osMutexRelease(myMutex04Handle);	// mutex 4 off
	if (info.memory_saved[memory_no-1] == 1)		// if there is memory saved
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void SD_point_to_memory(int position, int value)			//save points read from SD card to memory
{
	osMutexWait(myMutex04Handle, osWaitForever);			//mutex 4 on
	int memory_no = info.memory_number;						//find the memory number the graph corresponds to
	info.array_of_points[memory_no-1][position] = value;		//store the value into the memory array, in the corresponding position
	osMutexRelease(myMutex04Handle);	// mutex 4 off
}

int get_speed()		// gets speed variable
{
	osMutexWait(myMutex04Handle, osWaitForever);	// mutex 4 on
	int temp = info.speed_variable;		// temporarily stores speed
	osMutexRelease(myMutex04Handle);	// mutex 4 off
	return temp;		// return speed
}

void set_speed(int speed)	// set speed variable
{
	osMutexWait(myMutex04Handle, osWaitForever);	// mutex 4 on
	info.speed_variable = speed;		// set speed variable to input
	osMutexRelease(myMutex04Handle);	// mutex 4 off
	if (get_debug())
	{
		printf("%sSpeed is now %d\n%s", CYAN, info.speed_variable, WHITE);
	}
}

void increment_speed_plus()		// increment speed to make it faster
{
	osMutexWait(myMutex04Handle, osWaitForever);	// mutex 4 on
	if (info.speed_variable > 1)	// if speed variable is greater than 1
	{
		info.speed_variable -= 1;	// decrease speed variable by 1
	}
	osMutexRelease(myMutex04Handle);	// mutex 4 off
	if (get_debug())
	{
		printf("%sSpeed is now %d\n%s", CYAN, info.speed_variable, WHITE);
	}
}

void increment_speed_minus()	// increment speed to make it slower
{
	osMutexWait(myMutex04Handle, osWaitForever);	// mutex 4 on
	if (info.speed_variable < 50)	// if speed variable is less than 50
	{
		info.speed_variable += 1;	// increase speed variable by 1
	}
	osMutexRelease(myMutex04Handle);	// mutex 4 off
	if (get_debug())
	{
		printf("%sSpeed is now %d\n%s", CYAN, info.speed_variable, WHITE);
	}
}
