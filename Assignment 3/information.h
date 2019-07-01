/*
 * information.h
 *
 *  Created on: 23 May 2018
 *      Author: Joshua Beverley (c3195884), Radhika Feron (c3256870)
 */


#ifndef ASS_03_DEBUG_H_
#define ASS_03_DEBUG_H_

#include "Ass-03.h"
#define XSIZE 245
#define MEM_NO 5

typedef struct{
	int debug_variable;
	int stop_variable;
	int speed_variable;
	int memory_number;
	int array_of_points[MEM_NO][XSIZE];
	int temp_array[XSIZE];
	int memory_saved[MEM_NO];
} info_data;
info_data info;

extern void info_init();
extern int get_debug();
extern void set_debug(int var);
extern int get_stop();
extern void set_stop(int stop);
extern void change_memory(int memory_input);
extern char* get_memory();
extern int get_array_points_value(int position);
extern void set_temp_mem_value(int value, int position);
extern void temp_to_memory();
extern int memory_save_state();
extern void SD_point_to_memory(int position, int value);
extern int get_speed();
extern void set_speed(int speed);
extern void increment_speed_plus();
extern void increment_speed_minus();

#endif /* ASS_03_DEBUG_H_ */
