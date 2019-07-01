/*
 * drawLCD.h
 *
 *  Created on: 23 May 2018
 *      Author: Joshua Beverley (c3195884) & Radhika Feron c3256870
 */

#ifndef DRAWLCD_H_
#define DRAWLCD_H_

extern void screen_init();
extern void drawaxis(int speed);
void drawplay(int xpos, int ypos);
void drawstop(int xpos, int ypos);
void drawsave(int xpos, int ypos);
void drawload(int xpos, int ypos);
void drawerase(int xpos, int ypos);
void drawmem(int xpos, int ypos);
void drawspeedincrease(int xpos, int ypos);
void drawspeeddecrease(int xpos, int ypos);
extern void wipe_screen();

#endif /* DRAWLCD_H_ */
