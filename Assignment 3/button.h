/*
 * button.h
 *
 *  Created on: 6Jun.,2018
 *      Author: c3195884
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

extern int button_select(int loop, int y_disp, int x_disp);

#endif /* BUTTON_H_ */
