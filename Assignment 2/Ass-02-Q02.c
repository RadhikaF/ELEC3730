//     $Date: 2018-03-26 08:32:18 +1100 (Mon, 26 Mar 2018) $
// $Revision: 1217 $
//   $Author: Peter $

#include "Ass-02.h"
//#define x_pos 5 44 49 88 93 132 137 176 181 220 225 264 269 307
//#define y_pos 5 55 60 99 104 143 148 187 192 231
//#define X_LEN 14
//#define Y_LEN 10
#define BUTTON_NUM 24
//#define FAR_RIGHT 271
#define NUM_WIDTH 14
#define FAR_LEFT 10
#define MAX_NUMS 20
#define FAR_RIGHT FAR_LEFT + 1 + (NUM_WIDTH * MAX_NUMS)
#define FUNC_NUM 22
#define BINARY_BASE 2 /*Defining binary base*/
#define OCTAL_BASE 8  /*Defining octal base*/
#define DECIMAL_BASE 10 /*Defining decimal base*/
#define HEX_BASE 16    /*Defining hexa-decimal base*/
int debug_indicator;
double ans;
#define CALCULATOR_BUF_SIZE 50
#define MAX 50
#define MAX_BRACKET 10
#define CHARACTER_TO_SEARCH " "
#define MAX_VALUE 1000000000000000000000


//
// REPLACE THE EXAMPLE CODE WITH YOUR CODE 
//

void CalculatorInit(void)
{

	// Initialize and turn on LCD and calibrate the touch panel
	BSP_LCD_Init();
	BSP_LCD_DisplayOn();
	BSP_TP_Init();

	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

	BSP_LCD_DrawRect(5, 5, 310, 50);
	BSP_LCD_DrawRect(5, 60, 45, 40);
	BSP_LCD_DrawRect(5, 105, 45, 40);
	BSP_LCD_DrawRect(5, 150, 45, 40);
	BSP_LCD_DrawRect(5, 195, 45, 40);
	BSP_LCD_DrawRect(58, 60, 45, 40);
	BSP_LCD_DrawRect(58, 105, 45, 40);
	BSP_LCD_DrawRect(58, 150, 45, 40);
	BSP_LCD_DrawRect(58, 195, 45, 40);
	BSP_LCD_DrawRect(111, 60, 45, 40);
	BSP_LCD_DrawRect(111, 105, 45, 40);
	BSP_LCD_DrawRect(111, 150, 45, 40);
	BSP_LCD_DrawRect(111, 195, 45, 40);
	BSP_LCD_DrawRect(164, 60, 45, 40);
	BSP_LCD_DrawRect(164, 105, 45, 40);
	BSP_LCD_DrawRect(164, 150, 45, 40);
	BSP_LCD_DrawRect(164, 195, 45, 40);
	BSP_LCD_DrawRect(217, 60, 45, 40);
	BSP_LCD_DrawRect(217, 105, 45, 40);
	BSP_LCD_DrawRect(217, 150, 45, 40);
	BSP_LCD_DrawRect(217, 195, 45, 40);
	BSP_LCD_DrawRect(270, 60, 45, 40);
	BSP_LCD_DrawRect(270, 105, 45, 40);
	BSP_LCD_DrawRect(270, 150, 45, 40);
	BSP_LCD_DrawRect(270, 195, 45, 40);

	//char multiply = 158;
	//char divide = 246;
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAt(27, 68, (uint8_t*) "7", CENTER_MODE);
	BSP_LCD_DisplayStringAt(27, 113, (uint8_t*) "4", CENTER_MODE);
	BSP_LCD_DisplayStringAt(27, 158, (uint8_t*) "1", CENTER_MODE);
	BSP_LCD_DisplayStringAt(27, 203, (uint8_t*) "0", CENTER_MODE);
	BSP_LCD_DisplayStringAt(80, 68, (uint8_t*) "8", CENTER_MODE);
	BSP_LCD_DisplayStringAt(80, 113, (uint8_t*) "5", CENTER_MODE);
	BSP_LCD_DisplayStringAt(80, 158, (uint8_t*) "2", CENTER_MODE);
	BSP_LCD_DisplayStringAt(80, 203, (uint8_t*) ".", CENTER_MODE);
	BSP_LCD_DisplayStringAt(133, 68, (uint8_t*) "9", CENTER_MODE);
	BSP_LCD_DisplayStringAt(133, 113, (uint8_t*) "6", CENTER_MODE);
	BSP_LCD_DisplayStringAt(133, 158, (uint8_t*) "3", CENTER_MODE);
	BSP_LCD_DisplayStringAt(133, 203, (uint8_t*) "=", CENTER_MODE);
	BSP_LCD_DisplayStringAt(186, 68, (uint8_t*) "+", CENTER_MODE);
	BSP_LCD_DisplayStringAt(186, 113, (uint8_t*) "-", CENTER_MODE);
	BSP_LCD_DisplayStringAt(186, 158, (uint8_t*) "(", CENTER_MODE);
	BSP_LCD_DisplayStringAt(186, 203, (uint8_t*) "CL", CENTER_MODE);
	BSP_LCD_DisplayStringAt(239, 68, (uint8_t*) "*", CENTER_MODE);
	BSP_LCD_DisplayStringAt(239, 113, (uint8_t*) "/", CENTER_MODE);
	BSP_LCD_DisplayStringAt(239, 158, (uint8_t*) ")", CENTER_MODE);
	BSP_LCD_SetFont(&Font12);
	BSP_LCD_DisplayStringAt(239, 209, (uint8_t*) "DEBUG", CENTER_MODE);
	BSP_LCD_DisplayStringAt(292, 74, (uint8_t*) "BLUE", CENTER_MODE);
	BSP_LCD_DisplayStringAt(292, 119, (uint8_t*) "GREEN", CENTER_MODE);
	BSP_LCD_DisplayStringAt(292, 164, (uint8_t*) "RED", CENTER_MODE);
	BSP_LCD_DisplayStringAt(292, 209, (uint8_t*) "|ANS|", CENTER_MODE);
}

void CalculatorProcess(void)
{
	uint16_t linenum = 0;		//TODO Unused
	// variable for finding position along calculator window
	BSP_LCD_SetFont(&Font24);
	int counter = FAR_LEFT;
	int screencount = 0;
	int lastypos1 = 0;
	int lastypos2 = 0;
	int lastxpos1 = 0;
	int lastxpos2 = 0;
	int stringlen = 0;
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	int debug = 1;
	//char temp[CALCULATOR_BUF_SIZE] = {0};
	char answer[CALCULATOR_BUF_SIZE] = {0};
	//char strans[CALCULATOR_BUF_SIZE] = {0};
	int a = 0;
	int b = 0;
	int c = 0;												// initialise loop variables

	char input[MAX];												// input without brackets
	strcpy(input, "");
	int inputlen = 0;												// input length
	char bracket[MAX_BRACKET][MAX];											// where stuff inside brackets is stored
	double bracket_result[MAX_BRACKET];										// answer from brackets
	int bracketlen[MAX_BRACKET];												// bracket length
	for (l = 0; l < MAX_BRACKET; l++)			// l is the length of the stringlist
	{
		strcpy(bracket[l], "");
		bracket_result[l] = 0.0;
		bracketlen[l] = 0;
	}

	int length = 0;														// length of input string
	int bracketcounter = 0;											// how many brackets

	char stringlist[MAX];											// contains operations
	strcpy(stringlist, "");
	double numbersused[MAX];										// contains doubles
	for (l = 0; l <= MAX_BRACKET; l++)			// l is the length of the stringlist
	{
		numbersused[l] = 0.0;
	}
	char tempstring[MAX];
	strcpy(tempstring, "");
	double no1 = 0;
	double no2 = 0;

	double finalanswer = 0.0;										// final answer
	int lastop = 0;
	char prevans[MAX];												// previous answer
	strcpy(prevans, "0");
	int decimalcount = 0;											// detects how many decimal points entered


	const calcdata calculator_list[] =
	{
			{"7", 5, 50, 60, 100, 0},		//TODO the declaration of this struct type has pointers not integers, that's the warning
			{"4", 5, 50, 105, 145, 0},		//TODO create this in the calcinit function, and pass the pointer to this one
			{"1", 5, 50, 150, 190, 0},
			{"0", 5, 50, 195, 235, 0},
			{"8", 58, 103, 60, 100, 0},
			{"5", 58, 103, 105, 145, 0},
			{"2", 58, 103, 150, 190, 0},
			{".", 58, 103, 195, 235, 5},
			{"9", 111, 156, 60, 100, 0},
			{"6", 111, 156, 105, 145, 0},
			{"3", 111, 156, 150, 190, 0},
			{"=", 111, 156, 195, 235, 9},
			{"+", 164, 209, 60, 100, 1},
			{"-", 164, 209, 105, 145, 1},
			{"(", 164, 209, 150, 190, 2},
			{"p", 164, 209, 195, 235, 5},
			{"*", 217, 262, 60, 100, 1},
			{"/", 217, 262, 105, 145, 1},
			{")", 217, 262, 150, 190, 3},
			{"d", 217, 262, 195, 235, 6},
			{"b", 270, 315, 60, 100, 7},
			{"g", 270, 315, 105, 145, 7},
			{"r", 270, 315, 150, 190, 7},
			{"a", 270, 315, 195, 235, 4}
	};
	/*char str1[] = "hello";
	char str2[] = "world";
	strcat(str2, str1);
	printf("strings: %s, %s\n", str1, str2);
	 */
	while (1)
	{
		CommandLineParserProcess();
		if (delay(lastxpos1, lastxpos2, lastypos1, lastypos2, screencount) == 1)
		{
			screencount = 0;
			for (int k = 0; (k < BUTTON_NUM); k++)
			{
				if ((display.y >= calculator_list[k].ypos1) && (display.y <= calculator_list[k].ypos2) &&
						(display.x >= calculator_list[k].xpos1) && (display.x <= calculator_list[k].xpos2))		//TODO Fix the TODO above to solve this
				{
					lastxpos1 = calculator_list[k].xpos1;		//TODO Fix the TODO above the one above to solve this
					lastxpos2 = calculator_list[k].xpos2;
					lastypos1 = calculator_list[k].ypos1;
					lastypos2 = calculator_list[k].ypos2;
					if ((calculator_list[k].program_setting == 0))
					{
						if (lastop == 9)
						{
							counter = clr_screen();
						}
						//number or decimal point
						if (!((decimalcount == 1) && (calculator_list[k].program_setting == 5)))
						{
							// if there is a decimal point already in the number and the entered number was decimal, don't run
							if ((decimalcount == 0) && (calculator_list[k].program_setting == 5))
							{
								decimalcount = 1;
							}
							if ((lastop == 0) || (lastop == 1) || (lastop == 2) || (lastop == 9) || (lastop == 5))
							{
								lastop = 0;
								if (counter < FAR_RIGHT)
								{
									BSP_LCD_DisplayStringAt(counter, 25, (uint8_t*) calculator_list[k].calc_name, LEFT_MODE);
									counter += NUM_WIDTH;
									strcat(answer, calculator_list[k].calc_name);
									if (debug == 1)
									{
										printf("%s\n", answer);
									}
								}
							}

						}

					}
					else if (calculator_list[k].program_setting == 1)
					{
						//operation
						decimalcount = 0;
						if ((lastop == 0) || (lastop == 3) || (lastop == 4))
						{
							lastop = 1;
							strcat(answer, calculator_list[k].calc_name);
							if (debug == 1)
							{
								printf("%s\n", answer);
							}
							counter = clr_screen();
							if (counter < FAR_RIGHT)
							{
								BSP_LCD_SetFont(&Font12);
								BSP_LCD_DisplayStringAt(counter, 10, (uint8_t*) answer, LEFT_MODE);
								BSP_LCD_SetFont(&Font24);
							}
						}
					}
					else if (calculator_list[k].program_setting == 2)
					{
						decimalcount = 0;
						//operation
						if (lastop == 1)
						{
							lastop = 2;
							strcat(answer, calculator_list[k].calc_name);
							if (debug == 1)
							{
								printf("%s\n", answer);
							}
							counter = clr_screen();
							if (counter < FAR_RIGHT)
							{
								BSP_LCD_SetFont(&Font12);
								BSP_LCD_DisplayStringAt(counter, 10, (uint8_t*) answer, LEFT_MODE);
								BSP_LCD_SetFont(&Font24);
							}
						}
					}
					else if (calculator_list[k].program_setting == 3)
					{
						decimalcount = 0;
						//operation
						if ((lastop == 0) || (lastop == 4))
						{
							lastop = 3;
							strcat(answer, calculator_list[k].calc_name);
							if (debug == 1)
							{
								printf("%s\n", answer);
							}
							counter = clr_screen();
							if (counter < FAR_RIGHT)
							{
								BSP_LCD_SetFont(&Font12);
								BSP_LCD_DisplayStringAt(counter, 10, (uint8_t*) answer, LEFT_MODE);
								BSP_LCD_SetFont(&Font24);
							}
						}
					}
					else if (calculator_list[k].program_setting == 4)
					{
						//answer entered
						if ((lastop == 1) || (lastop == 2))
						{
							lastop = 4;
							strcat(answer, prevans);
							if (debug == 1)
							{
								printf("%s\n", answer);
							}
							counter = clr_screen();
							if (counter < FAR_RIGHT)
							{
								BSP_LCD_SetFont(&Font12);
								BSP_LCD_DisplayStringAt(counter, 10, (uint8_t*) answer, LEFT_MODE);
								BSP_LCD_SetFont(&Font24);
							}
						}
					}
					else if (calculator_list[k].program_setting == 5)
					{
						// clear entered
						if (lastop != 9)
						{
							lastop = 5;
							if (debug == 1)
							{
								printf("Screen Cleared\n");
							}
							counter = clr_screen();
							strcpy(answer, "");
						}
						colour(4);
					}
					else if (calculator_list[k].program_setting == 6)
					{
						//debug mode
						if (debug == 0)
						{
							debug = 1;
						}
						else
						{
							debug = 0;
						}
					}
					else if (calculator_list[k].program_setting == 7)
					{
						if (strcmp(calculator_list[k].calc_name, "b") == 0)
						{
							colour(1);
							if (debug == 1)
							{
								printf("Colour set to BLUE\n");
							}
						}
						else if (strcmp(calculator_list[k].calc_name, "g") == 0)
						{
							colour(2);
							if (debug == 1)
							{
								printf("Colour set to GREEN\n");
							}
						}
						else if (strcmp(calculator_list[k].calc_name, "r") == 0)
						{
							colour(3);
							if (debug == 1)
							{
								printf("Colour set to RED\n");
							}
						}
					}
					else if (calculator_list[k].program_setting == 9)
					{
						decimalcount = 0;
						//find solution
						if ((lastop == 0) || (lastop == 3) || (lastop == 4))
						{
							lastop = 9;

							length = strlen(answer);
							if (debug == 1)
							{
								printf("Full question: %s\n", answer);
								printf("Length: %d\n", length);
							}
							l = 0;
							int success = 1;
							for (a = 0; a < length; a++)									// loop until end of input
							{
								if ('(' == answer[a]) 										// if there is brackets
								{
									c = 0;
									for (b = a + 1; (')' != answer[b]); b++)				// loop until end bracket is encountered
									{
										if ((b) == length)									// if we've reached the end of the input and the end bracket wasn't here
										{
											//no end bracket, break loop
											if (debug == 1)
											{
												printf("Unequal amount of brackets\n");			// say so
											}
											a = length;										// then exit gracefully
											success = 0;
											strcpy(answer, "");
											break;
										}
										else if (answer[b] == '(')							// if nested brackets are used
										{
											if (debug == 1)
											{
												printf("Calculator can't do nested brackets\n");
											}
											a = length;										// say we don't do that, then exit gracefully
											success = 0;
											strcpy(answer, "");
											break;
										}
										else
										{
											a++;											// otherwise, increment i
										}
										bracket[bracketcounter][c] = answer[b];				// save the character to the brackets string
										c++;
									}
									printf("%s\n", bracket[bracketcounter]);
									for (b = 0; b <= 3; b++)
									{
										bracket[bracketcounter][c + b] = NULL;
									}
									strcat(bracket[bracketcounter], "+0");
									printf("%s\n", bracket[bracketcounter]);
									bracketcounter++;
									input[l] = '#';
									a++;
								}
								else
								{
									input[l] = answer[a];
								}
								l++;
							}
							printf("%s\n", input);
							for (c = 0; c <= 3; c++)
							{
								input[l+c] = NULL;
							}
							strcat(input, "+0");
							printf("%s\n", input);
							if (success == 1)
							{
								if (debug == 1)
								{
									printf("Question again: %s\n", answer);
									if (bracketcounter >= 1)
									{
										printf("No brackets: %s\n", input);
									}
								}
								for (a = 0; a < bracketcounter; a++)
								{
									if (debug == 1)
									{
										printf("Brackets %d: %s\n", a + 1, bracket[a]);
									}
								}
							}
							for (a = 0; a < bracketcounter; a++)							// cycle through all the brackets
							{
								bracketlen[a] = strlen(bracket[a]);							// find length of the bracket string
								c = 0;
								l = 0;
								for (b = 0; b < bracketlen[a]; b++)							// counts through elements of string
								{
									// j = loops
									// k = elements of stringlist string
									// l = consecutive numbers
									if ((bracket[a][b] == '+') || (bracket[a][b] == '-') || (bracket[a][b] == '*') || (bracket[a][b] == '/'))	// if an operation
									{
										stringlist[c] = bracket[a][b];						// put operation in next string
										numbersused[c] = atof(tempstring);
										strcpy(tempstring, "");

										c++;
										l = 0;
									}
									else
									{
										tempstring[l] = bracket[a][b];
										tempstring[l + 1] = NULL;
										l++;
									}

								}
								numbersused[c] = atof(tempstring);
								// do multiplication, division

								for (l = 0; l < c; l++)										// cycle through all the operations
								{
									if ((stringlist[l] == '*') || (stringlist[l] == '/'))
									{
										no1 = numbersused[l];
										for (b = 1; stringlist[l + b] == '@'; b++)
										{
											// increments until a non blank operator is reached
										}
										no2 = numbersused[l + b];
										if (stringlist[l] == '*')
										{
											numbersused[l + b] = no1 * no2;
										}
										else if (stringlist[l] == '/')
										{
											if (no2 == 0)
											{
												// can't divide by 0
												if (debug == 1)
												{
													printf("Can't divide by 0\n");
												}
												numbersused[l + b] = MAX_VALUE;
											}
											else
											{
												numbersused[l + b] = no1 / no2;
											}
										}
										stringlist[l] = '@';
										numbersused[l] = 0.0;
									}
								}
								//do addition, subtraction

								for (l = 0; l < c; l++)										// cycle through all the operations
								{
									if ((stringlist[l] == '+') || (stringlist[l] == '-'))
									{

										no1 = numbersused[l];
										for (b = 1; ((stringlist[l + b] == '@') && (b + l < c)); b++)
										{
											// increments until a non blank operator is reached
										}
										no2 = numbersused[l + b];
										if (stringlist[l] == '+')
										{
											numbersused[l + b] = no1 + no2;
										}
										else if (stringlist[l] == '-')
										{
											numbersused[l + b] = no1 - no2;
										}
										stringlist[l] = '@';
										numbersused[l] = 0.0;

									}
								}

								bracket_result[a] = numbersused[l];
								if (debug == 1)
								{
									printf("Bracket result %d: %lf\n", a+1, numbersused[l]);
								}
								strcpy(stringlist, "");
								no1 = 0;
								no2 = 0;
								for (l = 0; l <= c; l++)			// l is the length of the stringlist
								{
									numbersused[l] = 0.0;
								}
							}
							// brackets stuff complete, now complete rest of equation
							a = 0;
							c = 0;
							l = 0;
							bracketcounter = 0;
							inputlen = strlen(input);							// find length of the bracket string
							for (b = 0; b < inputlen; b++)							// counts through elements of string
							{
								// j = loops
								// k = elements of stringlist string
								// l = consecutive numbers
								if ((input[b] == '+') || (input[b] == '-') || (input[b] == '*') || (input[b] == '/'))	// if an operation
								{
									stringlist[c] = input[b];						// put operation in next string
									if (bracketcounter == 0)
									{
										numbersused[c] = atof(tempstring);
										strcpy(tempstring, "");
									}
									else
									{
										bracketcounter = 0;
									}
									c++;
									l = 0;
								}
								else if (input[b] == '#')	// if a result from brackets
								{
									numbersused[c] = bracket_result[a];
									a++;
									bracketcounter = 1;
								}
								else
								{
									tempstring[l] = input[b];
									tempstring[l + 1] = NULL;
									l++;
								}
							}
							numbersused[c] = atof(tempstring);

							// do multiplication, division

							for (l = 0; l < c; l++)										// cycle through all the operations
							{
								if ((stringlist[l] == '*') || (stringlist[l] == '/'))
								{
									no1 = numbersused[l];
									for (b = 1; stringlist[l + b] == '@'; b++)
									{
										// increments until a non blank operator is reached
									}
									no2 = numbersused[l + b];
									if (stringlist[l] == '*')
									{
										numbersused[l + b] = no1 * no2;
									}
									else if (stringlist[l] == '/')
									{
										if (no2 == 0)
										{
											// can't divide by 0
											if (debug == 1)
											{
												printf("Can't divide by 0\n");
											}
											numbersused[l + b] = MAX_VALUE;
										}
										else
										{
											numbersused[l + b] = no1 / no2;
										}
									}
									stringlist[l] = '@';
									numbersused[l] = 0.0;
								}
							}
							//do addition, subtraction

							for (l = 0; l < c; l++)										// cycle through all the operations
							{
								if ((stringlist[l] == '+') || (stringlist[l] == '-'))
								{

									no1 = numbersused[l];
									for (b = 1; ((stringlist[l + b] == '@') && (b + l < c)); b++)
									{
										// increments until a non blank operator is reached
									}
									no2 = numbersused[l + b];
									if (stringlist[l] == '+')
									{
										numbersused[l + b] = no1 + no2;
									}
									else if (stringlist[l] == '-')
									{
										numbersused[l + b] = no1 - no2;
									}
									stringlist[l] = '@';
									numbersused[l] = 0.0;

								}
							}

							finalanswer = numbersused[l];
							strcpy(stringlist, "");
							no1 = 0;
							no2 = 0;
							for (l = 0; l <= c; l++)			// l is the length of the stringlist
							{
								numbersused[l] = 0.0;
							}
							counter = clr_screen();
							if (counter < FAR_RIGHT)
							{
								BSP_LCD_SetFont(&Font12);
								BSP_LCD_DisplayStringAt(counter, 10, (uint8_t*) answer, LEFT_MODE);
								BSP_LCD_SetFont(&Font24);
							}
							if ((finalanswer >= MAX_VALUE) && (debug == 1))
							{
								printf("Number too large to display as result. \n");
								BSP_LCD_DisplayStringAt(counter, 25, (uint8_t*) "inf", LEFT_MODE);
							}
							else
							{
								printf("Final answer result: %lf\n\n", finalanswer);
								sprintf(prevans, "%g", finalanswer);
								BSP_LCD_DisplayStringAt(counter, 25, (uint8_t*) prevans, LEFT_MODE);
								sprintf(prevans, "%g", fabs(finalanswer));
							}
							strcpy(answer, "");
						}
					}
					break;
				}
			}
		}
		else if (BSP_TP_GetDisplayPoint(&display) != 0)
		{
			if (screencount < 1000)
			{
				screencount++;
			}
		}
		else
		{
		}
	}
}

int clr_screen()
{
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(6, 6, 308, 48);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	return FAR_LEFT;
}

int delay(int lastxpos1, int lastxpos2, int lastypos1, int lastypos2, int counter)
{
	if ((BSP_TP_GetDisplayPoint(&display) == 0))
	{
		//if button is pressed
		if ((display.y >= lastypos1) && (display.y <= lastypos2) && (display.x >= lastxpos1) && (display.x <= lastxpos2))
		{
			if (counter >= 1000)
			{
				//button hasn't been registered as a press more than 100 times
				return 1;
			}
			//if it's the same button as last time and less than 100 times
			return 0;
		}
		//if it's not the same button as last time
		return 1;
	}
	return 0;
}

void colour(int colourinp)
{
	if (colourinp == 1)
	{
		BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	}
	else if (colourinp == 2)
	{
		BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	}
	else if (colourinp == 3)
	{
		BSP_LCD_SetTextColor(LCD_COLOR_RED);
	}
	else if (colourinp == 4)
	{
		BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	}
	BSP_LCD_SetFont(&Font24);
	BSP_LCD_DisplayStringAt(27, 68, (uint8_t*) "7", CENTER_MODE);
	BSP_LCD_DisplayStringAt(27, 113, (uint8_t*) "4", CENTER_MODE);
	BSP_LCD_DisplayStringAt(27, 158, (uint8_t*) "1", CENTER_MODE);
	BSP_LCD_DisplayStringAt(27, 203, (uint8_t*) "0", CENTER_MODE);
	BSP_LCD_DisplayStringAt(80, 68, (uint8_t*) "8", CENTER_MODE);
	BSP_LCD_DisplayStringAt(80, 113, (uint8_t*) "5", CENTER_MODE);
	BSP_LCD_DisplayStringAt(80, 158, (uint8_t*) "2", CENTER_MODE);
	BSP_LCD_DisplayStringAt(80, 203, (uint8_t*) ".", CENTER_MODE);
	BSP_LCD_DisplayStringAt(133, 68, (uint8_t*) "9", CENTER_MODE);
	BSP_LCD_DisplayStringAt(133, 113, (uint8_t*) "6", CENTER_MODE);
	BSP_LCD_DisplayStringAt(133, 158, (uint8_t*) "3", CENTER_MODE);
	BSP_LCD_DisplayStringAt(133, 203, (uint8_t*) "=", CENTER_MODE);
	BSP_LCD_DisplayStringAt(186, 68, (uint8_t*) "+", CENTER_MODE);
	BSP_LCD_DisplayStringAt(186, 113, (uint8_t*) "-", CENTER_MODE);
	BSP_LCD_DisplayStringAt(186, 158, (uint8_t*) "(", CENTER_MODE);
	BSP_LCD_DisplayStringAt(186, 203, (uint8_t*) "CL", CENTER_MODE);
	BSP_LCD_DisplayStringAt(239, 68, (uint8_t*) "*", CENTER_MODE);
	BSP_LCD_DisplayStringAt(239, 113, (uint8_t*) "/", CENTER_MODE);
	BSP_LCD_DisplayStringAt(239, 158, (uint8_t*) ")", CENTER_MODE);
	BSP_LCD_SetFont(&Font12);
	BSP_LCD_DisplayStringAt(239, 209, (uint8_t*) "DEBUG", CENTER_MODE);
	BSP_LCD_DisplayStringAt(292, 74, (uint8_t*) "BLUE", CENTER_MODE);
	BSP_LCD_DisplayStringAt(292, 119, (uint8_t*) "GREEN", CENTER_MODE);
	BSP_LCD_DisplayStringAt(292, 164, (uint8_t*) "RED", CENTER_MODE);
	BSP_LCD_DisplayStringAt(292, 209, (uint8_t*) "|ANS|", CENTER_MODE);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font24);
}

