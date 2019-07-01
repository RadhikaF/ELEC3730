/*
 * debug.h
 *
 *  Created on: 23 May 2018
 *      Author: Joshua Beverley (c3195884)
 */

#ifndef DEBUG_H_
#define DEBUG_H_

typedef struct
{
	int debug_variable;
	int stop_variable;
	int speed_variable;
} info_data;
info_data info;

extern void info_init();
extern void screen_init();
void drawplay(int xpos, int ypos);
void drawstop(int xpos, int ypos);
void drawsave(int xpos, int ypos);
extern int get_debug();
extern void set_debug(int var);
extern int get_stop();
extern void set_stop(int stop);

#endif /* DEBUG_H_ */
