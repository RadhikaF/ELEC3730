#include "Ass-03.h"


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
	int selected_button = 0;	// button selected and returned from the button_select() function
	int prev_button = 0;	// previous button pressed

	Coordinate display;

	osSignalWait(1,osWaitForever);
	safe_printf("Hello from Task 2 - Pulse Rate Application (touch screen input)\n");

	while (1)
	{
		if (getfp(&display) == 0)	// if a button is pressed on screen
		{
			selected_button = button_select(display.y, display.x, prev_button);		// button selected on the screen, integer returned based on the struct in button.h
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sSelected button = %d%s\n", BLUE, selected_button, WHITE);	// output the selected button to putty
			}
			prev_button = selected_button;		//
		}
	}
}

int play()
{
	//play
	if (get_debug())
	{
		safe_printf("play\n");	// tell putty that play was selected and successfully run
	}
	osDelay(100);	// 100ms delay
	set_stop(0);	// call the set function that will change the stop variable to 0, causing the wave to un-stop
	return 0;
}
int stop()
{
	//stop
	if (get_debug())
	{
		safe_printf("stop\n");	// tell putty that stop was selected and successfully run
	}
	osDelay(100);	// 100ms delay
	set_stop(1);	// call the set function that will change the stop variable to 1, causing the wave to stop
	return 0;
}
int save()
{
	//save
	if (get_debug())
	{
		safe_printf("save\n");		// tell putty that save was selected and successfully run
	}
	osDelay(100);				// 100ms delay
	set_stop(1);				// call the set function that will change the stop variable to 1, causing the wave to stop
	temp_to_memory();			// save the memory stored in temporary storage to memory-specific storage
	myWriteFile();				// save the memory stored in temporary storage to the SD card, according to memory selected
	return 0;
}
int load()
{
	//load
	if (get_debug())
	{
		safe_printf("load\n");				// tell putty that load was selected and successfully run
	}
	osDelay(100);					// 100ms delay
	set_stop(1);					// call the set function that will change the stop variable to 1, causing the wave to stop
	myReadFile();					// read memory from the SD card, based on memory selected
	draw_saved_graph();				// draw the saved graph on the screen
	return 0;
}
int erase()
{
	//func1
	if (get_debug())
	{
		safe_printf("erase\n");	// tell putty that erase was selected and successfully run
	}
	osDelay(100);	// 100ms delay
	set_stop(1);	// call the set function that will change the stop variable to 1, causing the wave to stop
	osDelay(100);
	wipe_screen();	// clear the screen
	return 0;
}
int memory()
{
	//change memory
	if (get_debug())
	{
		safe_printf("memory\n");	// tell putty that memory was selected and successfully run
	}
	osDelay(100);	// 100ms delay
	change_memory(0);	// toggle the selected memory between 1, 2, 3
	return 0;
}
int more_speed()
{
	//increase speed
	if (get_debug())
	{
		safe_printf("speed +\n");	// tell putty that the increase speed was selected and successfully run
	}
	osDelay(100);	// 100ms delay
	increment_speed_plus();		// increase the speed value by decreasing the speed value by 1
	return 0;
}
int less_speed()
{
	//decrease speed
	if (get_debug())
	{
		safe_printf("speed -\n");	// tell putty that the decrease speed was selected and successfully run
	}
	osDelay(100);	// 100ms delay
	increment_speed_minus();	// decrease the speed value by increasing the speed value by 1
	return 0;
}
