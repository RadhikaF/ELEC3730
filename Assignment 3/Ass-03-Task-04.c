#include "Ass-03.h"

// This is the task that reads the analog input. A buffer is divided in two to
// allow working on one half of the buffer while the other half is being
// loaded using the DMA controller.
//
// This task also plots the wave form to the screen.
//
// *** MAKE UPDATES TO THE CODE AS REQUIRED ***
//
// Note that there needs to be a way of starting and stopping the display.

uint16_t ADC_Value[1000];
FIL MyFile;
#define BUFF_SIZE 256	// max size
#define XOFF 69		// where the left edge of the display is, in LCD screen pixels
#define YOFF 63		// where the top edge of the display is, in LCD screen pixels
#define XSIZE 245	// the length of the x direction of the screen (left to right) in LCD pixels
#define YSIZE 152	// the length of the y direction of the screen (top to bottom) in LCD pixels

void Ass_03_Task_04(void const * argument)
{
	uint16_t i;
	HAL_StatusTypeDef status;
	uint16_t xpos=0;		// initialise the x position, y position and last x and y position variables
	uint16_t ypos=0;
	uint16_t last_xpos=0;
	uint16_t last_ypos=YSIZE/2;		// set last y position to middle of the y screen size
	int stop_state = 0;		// local stop state variable
	int speed = 10;		// local speed variable, used in calculations
	int old_speed = 10;		// the previous speed variable
	int speed_var = 0;		// local speed variable, used to store the return from the get speed function
	int j = 0;		// variable used in loops
	int sum = 0; 	// used to average the inputs from the ADC

	osSignalWait(1,osWaitForever);
	safe_printf("Hello from Task 4 - Analog Input (turn ADC knob or use pulse sensor)\n");

	// Draw a box to plot in
	osMutexWait(myMutex01Handle, osWaitForever);		// open mutex 1
	BSP_LCD_DrawRect(XOFF-1,YOFF-1,XSIZE+1,YSIZE+1);	// draw a rectangle with dimensions of the graphing area
	osMutexRelease(myMutex01Handle);		// close mutex 1

	// Start the conversion process
	status = HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&ADC_Value, 1000);
	if (status != HAL_OK)
	{
		safe_printf("ERROR: Task 4 HAL_ADC_Start_DMA() %d\n", status);
	}

	// Start main loop

	while (1)
	{
		stop_state = get_stop();	// get stop variable
		if (stop_state == 1)		// if stop variable = 1 (stop)
		{
			// loop will continue until play is hit (when stop_state == 0)
			osDelay(200);	// wait 200ms
			if (stop_function())	// if the stop function returns 1 (which means erase or load was the last button hit)
			{
				xpos=0;		// reset the position of the x and y coordinates to what they were when initialised
				ypos=0;
				last_xpos=0;
				last_ypos=YSIZE/2;
			}
		}
		else // if (state == 0)
		{
			old_speed = speed_var;		// update the old speed variable
			speed_var = get_speed();	// update the speed variable, by running the get speed function
			if (speed_var <= 10)		// if the speed variable is less than 10,
			{
				speed = speed_var;		// update the speed variable used for calculations
			}
			else
			{
				speed = 10;			// otherwise leave it at 10
				osDelay(10 * speed_var);	// include a delay that is 10 * the speed variable
				// this delay increases the time it takes this function to loop, thus decreasing the speed of the pulse monitor
			}
			if (speed_var != old_speed)		// if speed has changed
			{
				drawaxis(speed_var);		// run the function to redrawe the axis
				set_temp_mem_value(speed_var, XSIZE-1);
			}
			// Wait for first half of buffer
			osSemaphoreWait(myBinarySem05Handle, osWaitForever);
			osMutexWait(myMutex01Handle, osWaitForever);		// turn mutex on
			for(i = 0; i < 500; i = i + 50 * speed)		// if speed is less than 10, the speed variable will change and thus the number of times the function loops
			{
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);		// change LCD draw colour to white
				BSP_LCD_DrawVLine(XOFF+xpos,YOFF,YSIZE);	// draw a vertical white line to remove the last wave
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);		// change back to black
				if  (speed == 10)		// if speed is equal to 10, speed variable will be greater than 10
				{
					for (j = 0; j < 500; j++)		// this loop averages all the ADC values
					{
						sum += ADC_Value[j];
					}
					sum = sum / 500;
					ypos=(sum * YSIZE / 4096);
					sum = 0;
				}
				else		// else just find ypos like normal
				{
					ypos=((ADC_Value[i+500])*YSIZE/4096);
				}

				if (xpos < XSIZE)
				{
					BSP_LCD_DrawLine(XOFF+last_xpos,YOFF+last_ypos,XOFF+xpos,YOFF+ypos);	// draw line from last x, y pos to new x, y pos
					set_temp_mem_value(ypos, xpos);		// save the x and y positions in temporary memory in information.c
					set_temp_mem_value(speed_var, XSIZE-1);
					last_xpos=xpos;		// update last x and y values
					last_ypos=ypos;
					xpos++;		// update x position
				}
			}
			osMutexRelease(myMutex01Handle);	// close mutex
			if (last_xpos>=(XSIZE-1))		// if the last x position is greater than the length of the display area, reset x and last x position
			{
				xpos=0;
				last_xpos=0;
			}
			// Wait for second half of buffer
			osSemaphoreWait(myBinarySem06Handle, osWaitForever);		// all same as above
			HAL_GPIO_WritePin(GPIOD, LD4_Pin, GPIO_PIN_SET);
			osMutexWait(myMutex01Handle, osWaitForever);
			for(i = 0; i < 500; i = i + 50 * speed)
			{
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				BSP_LCD_DrawVLine(XOFF+xpos,YOFF,YSIZE);
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				if  (speed == 10)
				{
					for (j = 500; j < 1000; j++)
					{
						sum += ADC_Value[j];
					}
					sum = sum / 500;
					ypos=(sum * YSIZE / 4096);
					sum = 0;
				}
				else
				{
					ypos=((ADC_Value[i+500])*YSIZE/4096);
				}
				if (xpos < XSIZE)
				{
					BSP_LCD_DrawLine(XOFF+last_xpos,YOFF+last_ypos,XOFF+xpos,YOFF+ypos);	// draw line from last x, y pos to new x, y pos
					set_temp_mem_value(ypos, xpos);		// save the x and y positions in temporary memory in information.c
					set_temp_mem_value(speed_var, XSIZE-1);
					last_xpos=xpos;		// update last x and y values
					last_ypos=ypos;
					xpos++;		// update x position
				}
			}
			osMutexRelease(myMutex01Handle);
			if (last_xpos>=(XSIZE-1))
			{
				xpos=0;
				last_xpos=0;
			}
			HAL_GPIO_WritePin(GPIOD, LD4_Pin, GPIO_PIN_RESET);
			BSP_LCD_DrawRect(XOFF-1,YOFF-1,XSIZE+1,YSIZE+1);	// redraw display screen area
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			BSP_LCD_FillRect(XOFF+XSIZE+1,YOFF-1,5,YSIZE+1);	// draw a white box on the far right to avoid any overshoot of the graph
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
		}
	}
}

// STEPIEN: Add callback functions to see if this can be used for double buffering equivalent

void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef* hadc)
{
	osSemaphoreRelease(myBinarySem05Handle);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_SET);
	osSemaphoreRelease(myBinarySem06Handle);
	HAL_GPIO_WritePin(GPIOD, LD3_Pin, GPIO_PIN_RESET);
}

int myReadFile(void)
{
	static int* rtext;
	int bytesread;
	rtext = (int * ) malloc (BUFF_SIZE * sizeof(int));		//allocate memory
	if(!rtext)		//check if memory is successfully allocated
		return 0;
	FRESULT res;
	char read_file[12] = {0};		//char array that will contain the file name
	strcpy(read_file, get_memory());		//retrieve the memory slot name and number and store in read_file string
	if (get_debug())		//if debug is on, print out the file name retrieved
	{
		safe_printf("File to be read is %s\n", read_file);
	}
	uint8_t temp[BUFF_SIZE];		//will contain the temporary path to read the file from
	strcpy(temp, "/Pulse/");		//memory files are contained in the Pulse folder
	strcat(temp, read_file);		//add file name to path
	strcpy(read_file,temp);			
	
	//check if file exists
	/*if ((res = f_stat(read_file, &fno)) != FR_OK){			//check for the existence of the file
		if (res == FR_NO_FILE){						//if no file is found, print error
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: The file does not exists%s\n",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		else{
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR checking existence of file%s\n",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		return res;
	}
	if (fno.fattrib & AM_ARC){		//if source is a file
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%s"%s" exists %s\n",YELLOW,read_file,WHITE);		//source is a file
		}
	}
	else{
		safe_printf("%sERROR%s\n",YELLOW,read_file,WHITE);		//source is a file
		return res;
	}*/

	if((res = f_open(&MyFile, read_file, FA_READ)) != FR_OK)		//open the file
	{
		safe_printf("%sERROR: Opening '%s'\n%s",RED, read_file,WHITE);		//print error if it fails
		return 0;
	}
	if (get_debug())		//if debug is on
	{
		safe_printf("Task 4: Opened file '%s'\n", read_file);
	}
	// Read data from file
	if ((res = f_read(&MyFile, rtext, sizeof(int)*XSIZE, &bytesread)) != FR_OK)		//read the memory file
	{
		safe_printf("%sERROR: Reading '%s'\n%s",RED, read_file,WHITE);		//if it fails, print error
		f_close(&MyFile);		//close file
		return 0;
	}
	if (get_debug())		//if debug is on
	{
		safe_printf("Bytes read: %d\n", bytesread);		//print the number of bytes read
		safe_printf("First element: %d\n", rtext[0]);	//print the first element
	}
	for (int i = 0; i<bytesread/sizeof(int);i++){
		SD_point_to_memory(i, rtext[i]);			//store each element in memory
	}
	// Close file
	f_close(&MyFile);

	return 1;
}

void myWriteFile()
{
	char write_file[12] = {0};			//will store the file name to be written
	strcpy(write_file, get_memory());	//get memory number and file name, and store in write_file
	if (get_debug())		//If debug is on
	{
		safe_printf("File to be written is %s\n", write_file);		//print file name to be written
	}
	FRESULT res;
	UINT byteswritten = 0, bytesw;
	int values[XSIZE+1] = {0};
	FILINFO fno;
	//check if the folder "Pulse" exists
	if ((res = f_stat("/Pulse", &fno)) != FR_OK){			//if an error occurs while checking folder existence
		if (res == FR_NO_FILE){			//if no 'Pulse' folder was found
			safe_printf("%sThe folder 'Pulse' does not exist in the root directory. Creating...%s\n",YELLOW,WHITE);
			//Creating folder 'Pulse' in the root directory
			if((res = f_mkdir("/Pulse")) != FR_OK){		//make a new directory named Pulse, if it fails
				safe_printf("%sERROR: New 'Pulse' folder could not be created\n%s",RED,WHITE);		//print error
				return;
			}
			else{
				safe_printf("> New folder: %s\n", "/Pulse");
			}
		}
		else{		//else if different error occurred
			safe_printf("%sERROR checking existence of 'Pulse' folder: %d%s\n",RED, res,WHITE);		//print error
			return;
		}
	}
	else if (fno.fattrib & AM_DIR){		//if Pulse is an existing folder
		if (get_debug())
		{
			safe_printf("%s 'Pulse' is an existing folder. %s\n",PURPLE,WHITE);		//source is a folder
		}
	}
	else {		//else folder does not exist, print error
		safe_printf("%sERROR: 'Pulse' is not a file or folder%s\n",RED,WHITE);		//source is not a file or folder
		return;			//exit function
	}
	// "/Pulse/filename"
	uint8_t temp[BUFF_SIZE];		//will store the temporary path to the new file
	strcpy(temp, "/Pulse/");		//within Pulse folder
	strcat(temp, write_file);		//store path in temp, with file name
	strcpy(write_file,temp);		//copy temp into write_file
	for (int i = 0; i < XSIZE; i++)
	{
		values[i] = get_array_points_value(i);		//get the values stored in memory for the graph
	}
	// Open file to write
	if((res = f_open(&MyFile, write_file, FA_CREATE_ALWAYS | FA_WRITE)) != FR_OK)
	{
		safe_printf("%sERROR: Opening '%s'\n%s", RED,write_file,WHITE);		//print error if it fails
		return;
	}
	if (get_debug())		//if debug is on
	{
		safe_printf("%sOpened file '%s'\n%s", YELLOW,write_file,WHITE);		//print success of opening file
	}
	// Write to file
	if ((res = f_write(&MyFile, values,sizeof(int)*XSIZE, &bytesw)) != FR_OK)		//if writing failed
	{
		safe_printf("%sERROR: Writing '%s'\n%s",RED, write_file,WHITE);		//print error
		f_close(&MyFile);		//close file
		return;
	}
	byteswritten += bytesw;		//save the total bytes written
	if (get_debug())		//if debug is on
	{
		safe_printf("%sWritten: %d bytes\n%s",YELLOW, byteswritten,WHITE);		//print the bytes written
	}
	// Close file
	res = f_close(&MyFile);
	if (get_debug())		//if debug is on
	{
		safe_printf("%sFile closed: res = %d\n%s",YELLOW,res,WHITE);		//print closing of file
	}
	return;
}

void draw_saved_graph()
{
	osMutexWait(myMutex01Handle, osWaitForever);
	int values[XSIZE+1] = {0};
	int speed = get_array_points_value(XSIZE-1);
	int check = 1;
	wipe_screen();
	osDelay(50);
	for (int i = 0; i < XSIZE; i++)
	{
		values[i] = get_array_points_value(i);
		if (values[i] == -1)
		{
			break;
		}
		if (check)
		{
			check = 0;
		}
		else
		{
			BSP_LCD_DrawLine(XOFF+i-1,YOFF+values[i-1],XOFF+i,YOFF+values[i]);
		}
	}
	if (get_debug())
	{
		printf("%ssaved speed = %d\n%s", CYAN, speed, WHITE);
	}
	drawaxis(speed);
	set_speed(speed);
	osMutexRelease(myMutex01Handle);
}
