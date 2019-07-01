//     $Date: 2018-03-26 08:32:18 +1100 (Mon, 26 Mar 2018) $
// $Revision: 1217 $
//   $Author: Peter $

// Assignment 2 include file

#ifndef ASS_02_H_
#define ASS_02_H_

// Standard includes
#ifdef STM32F407xx
#include "stm32f4xx_hal.h"
#include "openx07v_c_lcd.h"
#include "touch_panel.h"
#else
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include <math.h>

//Data type
typedef struct{
	int8_t *NameString; //Command string
	int (*Function_p)(uint8_t ArgNum,uint8_t *ArgStrings[]); //Function pointer
	int8_t *HelpString; //Help information
} command_s;
typedef struct
{
	int8_t *calc_name;
	int xpos1;
	int xpos2;
	int ypos1;
	int ypos2;
	int program_setting;
} calcdata;

// Assignment main
extern void Ass_02_Main(void);

//Question 1 Functions
int AddFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int SubFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int MulFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int DivFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int CosFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int SinFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int ExpFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int LnFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int LogFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int PowFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int SqrtFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int FactorialFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int DebugFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int ComplexFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int HelpFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int SinhFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int CoshFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int TanhFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int ConvertFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int AndFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int NumberSystemCheck(uint8_t type[], uint8_t argument[]);
int OrFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int XorFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int ComplementFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
void print_result(int result);
int IsNumeric(const uint8_t **ArgStrings[],const int pos);
int IsOperator(const uint8_t **ArgStrings[],const int pos);
void push(const uint8_t **ArgStrings[],const int pos);
char* pop();
int precedence(char symbol);
int RealFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int ImaginaryFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
int PolarFunction(unsigned int ArgNum,uint8_t **ArgStrings[]);
// Question 1
extern void CommandLineParserInit(void);
extern void CommandLineParserProcess(void);


// Question 2
extern void CalculatorInit(void);
extern void CalculatorProcess(void);
int delay(int lastxpos1, int lastxpos2, int lastypos1, int lastypos2, int counter);
int clr_screen();
void colour(int colourinp);

// Library functions
#ifdef STM32F407xx
extern uint8_t BSP_TP_Init(void);
extern uint8_t BSP_TP_GetDisplayPoint(Coordinate *pDisplay);
#endif

#endif /* ASS_02_H_ */
