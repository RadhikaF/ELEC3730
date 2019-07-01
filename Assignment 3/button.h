/*
 * button.h
 *
 *  Created on: 6Jun.,2018
 *      Author: c3195884 & c3256870
 */

#ifndef BUTTON_H_
#define BUTTON_H_

typedef struct
{
	int xpos1;
	int xpos2;
	int ypos1;
	int ypos2;
	int program_setting;
	int (*Function)();
} structdata;

extern int button_select(int y_disp, int x_disp, int prev_button);
extern int stop_function();

#endif /* BUTTON_H_ */
