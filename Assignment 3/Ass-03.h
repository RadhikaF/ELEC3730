// Assignment 3 include file

#ifndef ASS_03_H_
#define ASS_03_H_

// Standard includes
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

#include "stm32f4xx_hal.h"
#include "usart.h"
#include "touch_panel.h"
#include "openx07v_c_lcd.h"
#include "fatfs.h"
#include "adc.h"

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <math.h>
#include <time.h>

// OS handles
extern osThreadId defaultTaskHandle;
extern osThreadId myTask02Handle;
extern osThreadId myTask03Handle;
extern osThreadId myTask04Handle;
extern osTimerId myTimer01Handle;

extern osSemaphoreId myBinarySem01Handle;
extern osSemaphoreId myBinarySem02Handle;
extern osSemaphoreId myBinarySem03Handle;
extern osSemaphoreId myBinarySem04Handle;
extern osSemaphoreId myBinarySem05Handle;
extern osSemaphoreId myBinarySem06Handle;
extern osMessageQId myQueue01Handle;
extern osMessageQId myQueue02Handle;
extern osMutexId myMutex01Handle; // Protect LCD
extern osMutexId myMutex02Handle; // Protect console output
extern osMutexId myMutex03Handle;
extern osMutexId myMutex04Handle;
extern osMutexId myMutex05Handle;

// Assignment tasks
extern void Ass_03_Task_01(void const *argument);
extern void Ass_03_Task_02(void const *argument);
extern void Ass_03_Task_03(void const *argument);
extern void Ass_03_Task_04(void const *argument);

// Task 1
void debug_function(unsigned int ArgNum, uint8_t **Arguments[]);
void speed_function(unsigned int ArgNum, uint8_t **Arguments[]);
int string_parser(char *inp, char ***array_of_words_p, char character_to_search);
FRESULT ls_function (unsigned int ArgNum, uint8_t **Arguments[]);
FRESULT cd_function(unsigned int ArgNum, uint8_t **Arguments[]);
FRESULT mkdir_function (unsigned int ArgNum, uint8_t **Arguments[]);
FRESULT cp_function (unsigned int ArgNum, uint8_t **Arguments[]);
FRESULT rm_function (unsigned int ArgNum, uint8_t **Arguments[]);
FRESULT delete_directory(uint8_t *path);
FRESULT HelpFunction(unsigned int ArgNum,uint8_t **Arguments[]);
FRESULT mov_function (unsigned int ArgNum, uint8_t **Arguments[]);
void memory_setting_function (unsigned int ArgNum, uint8_t **Arguments[]);


typedef struct{
	int8_t *NameString;
	int (*Function_p)(unsigned int ArgNum, uint8_t **Arguments[]);
	int8_t *HelpString;
	int Type;	// 0 indicates an FRESULT return, 1 indicates a void return, 2 an int return
} command_s;

// Task 2
int save();
int load();
int memory();
int more_speed();
int less_speed();
int erase();
int stop();
int play();

// Task 4
int myReadFile(void);
void myWriteFile();
void draw_saved_graph();

//Putty Colours
#define RED "\e[0;31m"
#define GREEN "\e[0;32m"
#define YELLOW "\e[0;33m"
#define BLUE "\e[0;34m"
#define PURPLE "\e[0;35m"
#define CYAN "\e[0;36m"
#define WHITE "\e[0;37m"

// Other files
#include "information.h"
#include "button.h"
#include "drawLCD.h"

// Library functions
extern uint8_t BSP_TP_Init(void);
extern uint8_t BSP_TP_GetDisplayPoint(Coordinate *pDisplay);

// Front panel input
extern uint8_t getfp(Coordinate *display);

// STEPIEN: Safe printf() to ensure that only one task can write to
//          the console at a time

#define safe_printf(fmt, ...) \
	osMutexWait(myMutex02Handle, osWaitForever); \
	printf(fmt, ##__VA_ARGS__); \
	osMutexRelease(myMutex02Handle);

//
// ADD YOUR CODE
//

#endif /* ASS_03_H_ */
