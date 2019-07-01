//     $Date: 2018-03-26 08:32:18 +1100 (Mon, 26 Mar 2018) $
// $Revision: 1217 $
//   $Author: Peter $

#include "Ass-02.h"
#ifdef STM32F407xx
#include "usart.h"
#endif
#define FUNC_NUM 26		//constant defining the number of functions contained in the commandlist[] structure
#define PI 3.141592654	//constant for PI
#define BINARY_BASE 2 /*Defining binary base*/
#define OCTAL_BASE 8  /*Defining octal base*/
#define DECIMAL_BASE 10 /*Defining decimal base*/
#define HEX_BASE 16    /*Defining hexa-decimal base*/
int debug_indicator;		//global variable used to enter debug mode
int complex_mode;		//global variable used to enter complex mode
double ans;		//global variable containing the previous answer (does not work for complex mode)


void CommandLineParserInit(void)
{
	// Print welcome message
	printf("\014");
	printf("ELEC3730 Assignment 2\n");
	printf("Command Line Parser Example\n");
	debug_indicator = 0;
	ans = 0;
	complex_mode = 0;
}

void CommandLineParserProcess(void)
{
	uint8_t c;
	const command_s Commandlist[] = {
			{"add", &AddFunction, "add <num 1> .. <num N>. For complex: numbers can be real (x) or imaginary (yi)"},
			{"sub", &SubFunction, "sub <num 1> <num 2>. For complex: numbers can be real (x) or imaginary (yi)"},
			{"mul", &MulFunction, "mul <num 1> .. <num N>. For complex: only two complex numbers allowed: mul <real1> <imaginary1> <real2> <imaginary2>"},
			{"div", &DivFunction, "div <num 1> <num 2>. For complex: only two complex numbers allow: div <real1> <imaginary1> <real2> <imaginary2>"},
			{"cos", &CosFunction, "cos <num 1>"},
			{"sin", &SinFunction, "sin <num 1>"},
			{"exp", &ExpFunction, "exp <num 1>"},
			{"ln", &LnFunction, "ln <num 1>"},
			{"log", &LogFunction, "log <num 1>"},
			{"pow", &PowFunction, "pow <num 1>"},
			{"sqrt", &SqrtFunction, "sqrt <num 1>"},
			{"fact", &FactorialFunction, "fact <num 1>"},
			{"sinh", &SinhFunction, "sinh <num 1>"},
			{"cosh", &CoshFunction, "cosh <num 1>"},
			{"tanh", &TanhFunction, "tanh <num 1>"},
			{"convert", &ConvertFunction, "convert <type (hex, oct, bin, dec)> <number>"},
			{"help", &HelpFunction, "Turn debug messages on or off"},
			{"debug", &DebugFunction, "Display help messages"},
			{"complex", &ComplexFunction, "Switch complex mode on/off"},
			{"and", &AndFunction, "and <type (hex, oct, bin, dec) <num 1> <num 2>"},
			{"or", &OrFunction, "or <type (hex, oct, bin, dec) <num 1> <num 2>"},
			{"xor", &XorFunction, "xor <type (hex, oct, bin, dec) <num 1> <num 2>"},
			{"comp", &ComplementFunction, "comp <type (hex, oct, bin, dec) <num 1>"},
			{"real", &RealFunction, "Complex mode only: real <real part> <imaginary part>"},
			{"imag", &ImaginaryFunction, "Complex mode only: imag <real part> <imaginary part>"},
			{"polar", &PolarFunction, "Complex mode only: polar <real part> <imaginary part>"},
			{NULL, NULL, NULL}
	};

	// Check for input and echo back
#ifdef STM32F407xx

	if (HAL_UART_Receive(&huart2, &c, 1, 0x0) == HAL_OK)
	{
		//printf(__LINE__);
		static char expression[200];			//will contain the string that is typed in putty
		static int position = 0;				//position in the string to add the character read
		HAL_GPIO_TogglePin(GPIOD, LD4_Pin); // Toggle LED4
		if(c == '\r'){			//if the last input character was enter, the end of the string has been reached
			printf("\n");
			expression[position] = '\0';			//place a null character at the end of the string
			char *inp;				//contain input string
			int max_size = 200;	// max size of string
			inp = (char *) malloc(max_size * sizeof(char));	// malloc
			inp = expression;			//let the input string point to the expression string read
			char ** array_of_words;		// output array of pointers to the start of each world
			array_of_words = (char * ) malloc (max_size * sizeof(char));
			int count;					//word count
			int j = 0;
			count = string_parser(inp, &array_of_words);
			if (count != 0)			//if words were found
			{
				if (debug_indicator == 1){
					printf("Word count = %d\n", count);
					for (j=0;j<count;j++)
					{
						printf("Word %d: %s\n", j+1, (array_of_words)[j]);
					}
				}
				//check if the command exists
				int position = -1;		//sentinel value will remain -1 if command not found
				for (int k = 0; k < FUNC_NUM; k++){		//cycle through all the function positions within the structure
					if (strcmp(array_of_words[0],Commandlist[k].NameString)==0){		//if a command is found
						position = k;		//store the current position within the structure
						break;		//exit for loop
					}
				}
				if (position == -1 && debug_indicator == 1){	//if the command doesn't exist and debug mode is on
					printf("Syntax Error: Command '%s' could not be found\n", array_of_words[0]);		//print error
				}
				else if (position == -1 && debug_indicator == 0){		//if debug mode is off
					printf("Syntax Error\n");		//print error
				}
				else{		//if the command exists
					int success = Commandlist[position].Function_p(count,&array_of_words);	//use the function at that position to calculate the result
					if (success == 0 && debug_indicator == 1){		//if the outcome of calling the function was unsuccessful (0), and debug mode is on
						printf("Command '%s' was not successfully completed\n", array_of_words[0]);		//print unsuccessful message
					}
					else if (debug_indicator == 1){		//else if it was successful, and debug mode is on
						printf("Command '%s' was successfully completed\n", array_of_words[0]);		//print success message
					}
				}
				//free allocated memory
				free(array_of_words[0]);
				free(array_of_words);
			}
			position = 0;
			expression[position] = '\0';
			printf("\n");
		}
		else{
			printf("%c", c);
			expression[position] = c;
			position++;
			//printf("Character: %c\n", c);
		}
	}
#else
	int max_size = 200;	// max size of string
	int numbers;		// very essential variable that must not be deleted
	char * inp;			// input string
	char ** array_of_words;		// output array of pointers to the start of each world
	inp = (char *) malloc(max_size * sizeof(char));	// malloc
	array_of_words = (char * ) malloc (max_size * sizeof(char));
	numbers = getline(&inp, &max_size, stdin);
	numbers--;	// numbers is one too large so subtract one
	inp[numbers] = ' ';		// last character in string is a return carriage so make it a space
	//printf("SERIAL: Got '%s'\n", inp);

	int count;
	int j = 0;
	//printf("--> '%s': ",inp);
	count = string_parser(inp, &array_of_words);
	if (count != 0)			//if words were found
	{
		if (debug_indicator == 1){
			printf("Word count = %d\n", count);
			for (j=0;j<count;j++)
			{
				printf("Word %d: %s\n", j+1, (array_of_words)[j]);
			}
		}
		//check if the command exists
		int position = -1;		//sentinel value will remain -1 if command not found
		for (int k = 0; k < FUNC_NUM; k++){		//cycle through all the function positions within the structure
			if (strcmp(array_of_words[0],Commandlist[k].NameString)==0){		//if a command is found
				position = k;		//store the current position within the structure
				break;		//exit for loop
			}
		}
		if (position == -1 && debug_indicator == 1){	//if the command doesn't exist and debug mode is on
			printf("Syntax Error: Command '%s' could not be found\n", array_of_words[0]);		//print error
		}
		else if (position == -1 && debug_indicator == 0){		//if debug mode is off
			printf("Syntax Error\n");		//print error
		}
		else{		//if the command exists
			int success = Commandlist[position].Function_p(count,&array_of_words);	//use the function at that position to calculate the result
			if (success == 0 && debug_indicator == 1){		//if the outcome of calling the function was unsuccessful (0), and debug mode is on
				printf("Command '%s' was not successfully completed\n", array_of_words[0]);		//print unsuccessful message
			}
			else if (debug_indicator == 1){		//else if it was successful, and debug mode is on
				printf("Command '%s' was successfully completed\n", array_of_words[0]);		//print success message
			}
		}
		//free allocated memory
		free(array_of_words[0]);
		free(array_of_words);
	}
#endif
}
int string_parser(char *inp, char ***array_of_words_p)
{
	char character_to_search = ' ';		// character that separates two words, in this case a space
	int counter = 1;		// counter used for for loops
	int no_of_words = 0;	// variable used to count number of words found in the string
	int characters = 0;		// variable used to count the number of characters (non spaces) found in the string
	char current_character = inp[counter];		// character in the string to be looked at in the for loop
	char prev_character = inp[counter - 1];		// previous character in the string that was looked at
	for (counter = 1; inp[counter] != NULL; counter++) // counter increments until it reaches the end of the string
	{
		if (prev_character != character_to_search) // if the previous character was NOT a space
		{
			characters++;	// increment 1 to number of characters, previous character was not a space
			if (current_character == character_to_search)	// if the current character is a space
			{
				no_of_words++;	// if prev is a character and current is a space, word just completed, increment number of words
			}
		}
		prev_character = inp[counter - 1];	// set the value of previous character with the updated counter value
		current_character = inp[counter];	// set the value of current character with the updated counter value
	}
	if ((prev_character != character_to_search) && (strlen(inp) != 0))	// checks the last value of the string, adds to word count if needed
	{
		// checks the last value of the string, adds to word count if needed
		// also checks the length of the string, if string is empty it is ignored by this statement
		characters++;	// increase 1 to number of characters, previous character was not a space
		no_of_words++;	// increase number of words by 1
	}
	//printf("\ncharacters: %d\nwords: %d\n", characters, no_of_words);

	characters = characters + no_of_words + 1; 	// add an extra 2 before mallocing just to make sure
	char * copy = (char * ) calloc (characters, sizeof(char));	// malloc enough memory for each character
	counter = 0;
	for (counter = 0; counter == characters; counter++)		// assigns every value in the calloc'd memory to null character
	{
		copy[counter] = '\0';
	}
	int length = -1;	// effectively characters + no_of_words - 1, starts at -1 as it is used as an array index, starting at 0
	int signal = 0; 	// alternative to previous number, set to 1 when there's a space
	counter = 0;
	//(**array_of_words_p) = (char * ) malloc (no_of_words);	// mallocs the memory for array of words, so it can be stored outside this function
	no_of_words = 0;	// resets and is incremented as words are entered into the array
	current_character = inp[counter];	// resets current character to first character

	for (counter = 0; inp[counter] != NULL; counter++)		// counter increments until it reaches the end of the string again
	{
		current_character = inp[counter];	// updates the current character with the current character
		if ((current_character != character_to_search))		// if current character is NOT a space
		{
			if (signal == 1 || counter == 0)	// if signal = 1 (previous character was a space),
				// or if this is the first iteration through the for loop and the first character is not a space
			{
				signal = 0;		// reset signal to 0 as the current character is not a space
				(*array_of_words_p)[no_of_words] = &copy[length + 1];	// save in array_of_words the address of the beginning of the current word
				// current word is located using length, which is incremented using the counter.
				no_of_words++;	// increment number of words as word has just started
				length++;		// increment length, to add a space
			}
			copy[length] = inp[counter];	// save in copy the character from the input
			length++;			// increment length, to add a character
		}
		else	// else current character is a space
		{
			signal = 1;		// change signal to 1, to indicate in the next iteration that the previous character was a space
		}
	}

	//printf("\n");
	return no_of_words;		// return the number of words in the string
}
int AddFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	double sum = 0;			//will contain the result
	double add;				//will contain the number to add to the result
	printf("-->Adding...\n");
	char substr[100];		//contain substring of a complex number (without the i)
	char temp[100];			//temporarily hold one argument string
	int size;				//size of the string
	if(complex_mode == 1){			//if in complex mode
		double real_sum = 0, imag_sum = 0;
		for(int i = 1; i < ArgNum; i++){			//cycle through the array
			sprintf(temp, "%s", (*ArgStrings)[i]);			//copy the string element into a new variable
			size = strlen(temp);				//find the length of the string
			if (temp[size-1] == 'i'){			//if it is a complex number (last value is an 'i')
				strncpy(substr, temp, size-1);			//copy the elements excluding the i to another string
				add = atof(substr); //change string to floating point number
				if (add == 0 && strcmp(substr,"0") != 0){		//check if the value is a number
					if (debug_indicator == 1){
						printf("***Syntax Error: One of the arguments is not a number\n");		//if it is not a number, print an error message
					}
					else {
						printf("***Syntax Error\n");
					}
					return 0;
				}
				imag_sum += add;			//add the imaginary value to the imaginary sum
			}
			else{
				add = atof(temp); //change string to floating point number
				if (add == 0 && strcmp(temp,"0") != 0){		//check if the value is a number
					if (debug_indicator == 1){
						printf("***Syntax Error: One of the arguments is not a number\n");		//if it is not a number, print an error message
					}
					else {
						printf("***Syntax Error\n");
					}
					return 0;			//return 0 if an error occurs
				}
				real_sum += add;		//add the real value to the real sum
			}
		}
		printf("Result: %.2f + %.2f i\n", real_sum, imag_sum);			//print the result
		return 1;
	}
	//else if complex mode is not selected
	if (ArgNum < 3){		//checks if there are less than 3 arguments (including the command argument)
		if (debug_indicator == 1){
			printf("***Syntax Error: Need at least 2 arguments\n");		//if false, then print error
		}
		else {
			printf("***Syntax Error\n");
		}
		return 0;
	}
	for (int i = 1; i< ArgNum; i++){			//cycle through the strings
		if (strcmp("ans", (*ArgStrings)[i]) == 0){			//if the argument is using the previous answer
			add = ans;						//let the number to be added equal the answer
		}
		else if (strcmp("PI", (*ArgStrings)[i]) == 0){			//if the argument is using PI
			add = PI;
		}
		else{
			add = atof((*ArgStrings)[i]); //change string to floating point number
			if (add == 0 && strcmp((*ArgStrings)[i],"0") != 0){		//check if the value is a number
				if (debug_indicator == 1){
					printf("***Syntax Error: One of the arguments is not a number\n");		//if it is not a number, print an error message
				}
				else {
					printf("***Syntax Error\n");
				}
				return 0;
			}
		}
		sum += add;			//add the value to the sum
	}
	printf("   Result: %f\n",sum);			//print the result
	ans = sum;			//save the answer
	return 1;
}
int RealFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	printf("-->Finding the real part of the complex number...\n");
	if(complex_mode == 1){		//if complex mode is on
		if (ArgNum != 3){			//the total arguments should be 2 excluding command argument
			if (debug_indicator == 1){
				printf("***Syntax Error: The required arguments was not met\n");			//print error
			}
			else{
				printf("***Syntax Error");
			}
			return 0;
		}
		int imag_count = 0, real_count = 0;  //count of the number of real/imaginary arguments encountered
		int size;		//will contain the size of a string
		double real_result;		//will contain the result
		char temp[100];		//temporarily contains the argument string focused on
		for(int i = 1; i < ArgNum; i++){			//cylce through the input arguments
			sprintf(temp, "%s", (*ArgStrings)[i]);			//copy argument to a string
			size = strlen(temp);		//find size of the string
			if (temp[size-1] == 'i'){		//if the last element is an i, then it is a imaginary number
				imag_count++;			//increment the counter for the number of imaginary numbers encountered
			}
			else{
				real_count++;			//increment the counter for the number of real numbers encountered
				real_result = atof(temp);
				if (real_result == 0 && strcmp(temp,"0") != 0){		//check if the value is a number
					if (debug_indicator == 1){
						printf("***Syntax Error: One of the arguments is not a number\n");		//if it is not a number, print an error message
					}
					else {
						printf("***Syntax Error\n");
					}
					return 0;
				}
			}
			if (imag_count != 1 && real_count != 1){			//if the input format is not of one real and one imaginary number
				if (debug_indicator == 1){
					printf("***Syntax Error: Too many real/imaginary numbers\n");
				}
				else{
					printf("***Syntax Error");
				}
				return 0;
			}
		}
		printf("Real part: %.2f\n", real_result);		//print result
	}
	else{		//if complex mode is not selected
		printf("***Error: Complex mode not selected\n"); // print error and return 0
		return 0;
	}
	return 1;
}
int ImaginaryFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	printf("-->Finding the imaginary part of the complex number...\n");
	if(complex_mode == 1){			//if complex mode is selected
		if (ArgNum != 3){			//the function accepts two arguments (excluding command argument)
			if (debug_indicator == 1){
				printf("***Syntax Error: The required arguments was not met\n");		//if this is not met, print error and exit
			}
			else{
				printf("***Syntax Error");
			}
			return 0;
		}
		int imag_count = 0, real_count = 0;  //count of the number of real/imaginary arguments encountered
		int size;		//will contain the size of a string
		double imag_result;		//will contain the result
		char temp[100];		//temporarily contains the argument string focused on
		char substr[100];
		for(int i = 1; i < ArgNum; i++){			//cycle through the input arguments
			sprintf(temp, "%s", (*ArgStrings)[i]);			//copy argument to a string
			size = strlen(temp);		//find size of the string
			if (temp[size-1] == 'i'){		//if the last element is an i, then it is a imaginary number
				imag_count++;			//increment the counter for the number of imaginary numbers encountered
				strncpy(substr, temp, size-1);		//copy the elements excluding the i to another string
				imag_result = atof(substr);				//change the string to float
				if (imag_result == 0 && strcmp(substr,"0") != 0){		//check if the value is a number
					if (debug_indicator == 1){
						printf("***Syntax Error: One of the arguments is not a number\n");		//if it is not a number, print an error message
					}
					else {
						printf("***Syntax Error\n");
					}
					return 0;
				}
			}
			else{
				real_count++;				//increment the counter for the number of real numbers encountered
			}
			if (imag_count != 1 && real_count != 1){		//if the format does not follow "one real one imaginary number"
				if (debug_indicator == 1){
					printf("***Syntax Error: Too many real/imaginary numbers\n");			//print error
				}
				else{
					printf("***Syntax Error");
				}
				return 0;
			}
		}
		printf("Imaginary part: %.2f\n", imag_result);			//print result
	}
	else{
		printf("***Error: Complex mode not selected\n");			//else if complex mode is not on, print error and return 0
		return 0;
	}
	return 1;
}
int PolarFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	printf("-->Finding the polar version...\n");
	if(complex_mode == 1){			//if the complex mode is selected
		if (ArgNum != 3){			//two arguments are required (excluding the command argument)
			if (debug_indicator == 1){
				printf("***Syntax Error: The required arguments was not met\n");		//print error
			}
			else{
				printf("***Syntax Error");
			}
			return 0;
		}
		int imag_count = 0, real_count = 0;  //count of the number of real/imaginary arguments encountered
		int size;		//will contain the size of a string
		double imag_result, real_result;		//will contain the result
		char temp[100];		//temporarily contains the argument string focused on
		char substr[100];
		for(int i = 1; i < ArgNum; i++){			//cylce through the input arguments
			sprintf(temp, "%s", (*ArgStrings)[i]);			//copy argument to a string
			size = strlen(temp);		//find size of the string
			if (temp[size-1] == 'i'){		//if the last element is an i, then it is a imaginary number
				imag_count++;				//increment the counter for the number of imaginary numbers encountered
				strncpy(substr, temp, size-1);		//copy the elements excluding the i to another string
				imag_result = atof(substr);			//change string to float
				if (imag_result == 0 && strcmp(substr,"0") != 0){		//check if the value is a number
					if (debug_indicator == 1){
						printf("***Syntax Error: One of the arguments is not a number\n");		//if it is not a number, print an error message
					}
					else {
						printf("***Syntax Error\n");
					}
					return 0;
				}
			}
			else{
				real_count++;			//increment the counter for the number of real numbers encountered
				real_result = atof(temp);			//change string to float
				if (real_result == 0 && strcmp(temp,"0") != 0){		//check if the value is a number
					if (debug_indicator == 1){
						printf("***Syntax Error: One of the arguments is not a number\n");		//if it is not a number, print an error message
					}
					else {
						printf("***Syntax Error\n");
					}
					return 0;
				}
			}
			if (imag_count != 1 && real_count != 1){			//if the format does not follow "one real one imaginary number"
				if (debug_indicator == 1){
					printf("***Syntax Error: Too many real/imaginary numbers\n");		//print error
				}
				else{
					printf("***Syntax Error");
				}
				return 0;
			}
		}
		double modulus = sqrt(real_result*real_result + imag_result*imag_result);		//calculate the modulus of the complex number
		double argument = atan(imag_result/real_result);								//calculate the argument of the complex number
		printf("Modulus: %.2f\n", modulus);				//print modulus result
		printf("Argument: %.2f\n", argument*180/PI);	//print argument result
	}
	else{
		printf("***Error: Complex mode not selected\n");		//else if the complex mode is off, print error and return 0
		return 0;
	}
	return 1;
}
int SubFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	double result, num1, num2;			//variables that will contain the final result, and the two arguments
	printf("-->Subtracting...\n");
	int size;			//contain the size of a string
	char temp[100];		//temporary variable to contain an argument string
	char substr[100];	//contain substring of the imaginary number
	int first_number = 1;	//
	int first_imag = 1;
	if(complex_mode == 1){
		double real_sum = 0, imag_sum = 0;
		for(int i = 1; i < ArgNum; i++){
			sprintf(temp, "%s", (*ArgStrings)[i]);			//store argument in temporary string
			size = strlen(temp);
			if (temp[size-1] == 'i'){				//if the last element is an i, then it is a imaginary number
				strncpy(substr, temp, size-1);		//copy the elements excluding the i to another string
				result = atof(substr);				//change string to floating point number
				if (result == 0 && strcmp(substr,"0") != 0){		//check if the value is a number
					if (debug_indicator == 1){
						printf("***Syntax Error: One of the arguments is not a number\n");		//if it is not a number, print an error message
					}
					else {
						printf("***Syntax Error\n");
					}
					return 0;
				}
				if(first_imag == 1){		//if it is the first imaginary number found, then let the result equal it
					imag_sum = result;
					first_imag++;		//increment first_imag to indicate one imaginary number has been found already (so if input is 4i 5i, result is 4i-5i)
				}
				else{
					imag_sum -= result;		//subtract the next imaginary number from the result
				}
			}
			else{
				result = atof(temp); //change string to floating point number
				if (result == 0 && strcmp(temp,"0") != 0){		//check if the value is a number
					if (debug_indicator == 1){
						printf("***Syntax Error: One of the arguments is not a number\n");		//if it is not a number, print an error message
					}
					else {
						printf("***Syntax Error\n");
					}
					return 0;
				}
				if (first_number == 1){		//if it is the first imaginary number found, then let the result equal it
					real_sum = result;
					first_number++;			//increment first_imag to indicate one real number has been found already (so if input is 4 5, result is 4-5)
				}
				else{
					real_sum -= result;			//subtract the next real number from the result
				}
			}
		}
		printf("Result: %.2f + %.2f i\n", real_sum, imag_sum);			//print result
		return 1;
	}	//if complex mode has not been selected
	if (ArgNum != 3){		//checks if there are 3 arguments (including the command argument)
		if (debug_indicator == 1){
			printf("***Syntax Error: Only 2 arguments allowed\n");		//if false, then print error
		}
		else {
			printf("***Syntax Error\n");
		}
		return 0;
	}
	if (strcmp("ans", (*ArgStrings)[1]) == 0){			//if the argument is using the previous answer
		num1 = ans;
	}
	else if (strcmp("PI", (*ArgStrings)[1]) == 0){		//if the argument is using PI
		num1 = PI;
	}
	else{
		num1 = atof((*ArgStrings)[1]);		//change string to floating point number
		if (num1 == 0 && strcmp((*ArgStrings)[1],"0") != 0){		//check if the value is a number
			if (debug_indicator == 1){
				printf("***Syntax Error: Argument 1 is not a number\n");		//if not, print error
			}
			else {
				printf("***Syntax Error\n");
			}
			return 0;
		}
	}
	if (strcmp("ans", (*ArgStrings)[2]) == 0){			//if the argument is using the previous answer
		num2 = ans;
	}
	else if (strcmp("PI", (*ArgStrings)[2]) == 0){			//if the argument is using PI
		num2 = PI;
	}
	else{
		num2 = atof((*ArgStrings)[2]);			//change string to floating point number
		if (num2 == 0 && strcmp((*ArgStrings)[2],"0") != 0){			//check if the value is a number
			if (debug_indicator == 1){
				printf("***Syntax Error: Argument 2 is not a number\n");		//if not print error
			}
			else {
				printf("***Syntax Error\n");
			}
			return 0;
		}
	}
	result = num1-num2;			//subtract the two values
	printf("   Result: %f\n", result);			//print result
	ans = result;			//save answer
	return 1;
}
int MulFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	double result = 1, num;			//store the result and the argument
	printf("-->Multiplying...\n");
	if (complex_mode == 1){			//if in complex mode
		if (ArgNum < 3){			//this function accepts minimum two arguments (excluding command argument)
			if (debug_indicator == 1){
				printf("***Syntax Error: Need at least 2 arguments (excluding command)\n");		//if false, then print error
			}
			else{
				printf("***Syntax Error\n");
			}
			return 0;
		}
		else if (ArgNum > 5){		//this function accepts maximum 4 arguments (only multiplying two complex numbers)
			if (debug_indicator == 1){
				printf("***Syntax Error: Need at most 4 arguments (excluding command)\n");		//if false, then print error
			}
			else{
				printf("***Syntax Error\n");
			}
			return 0;
		}
		int size;		//contain size of a string
		int first_imag = 0, first_real = 0;		//indicates whether the imaginary/real number found was the first of its respective type
		int real_count = 0, imag_count = 0,imag_real_count = 0;		//counts the number of occurrences of real/imaginary numbers, and imag_real_count indicates if the input format is of the form "imaginary real number"
		char temp[100], substr[100];
		double real1, imag1, real2, imag2;		//contains the real/imaginary components of the two complex numbers
		for(int i = 1; i < ArgNum; i++){		//cycle through the arguments
			sprintf(temp, "%s", (*ArgStrings)[i]);		//store argument in temporary string
			size = strlen(temp);			//find length of string
			if (temp[size-1] == 'i'){					//if the last element is an i, then it is a imaginary number
				imag_count++;			//increment the counter for the number of imaginary numbers encountered
				if (imag_count > 2){		//if more than one imaginary number is detected
					if (debug_indicator == 1){
						printf("***Syntax Error: More than one imaginary number detected\n");		//print an error message
					}
					else {
						printf("***Syntax Error\n");
					}
					return 0;
				}
				strncpy(substr, temp, size-1);			//copy the elements excluding the i to another string
				result = atof(substr); //change string to floating point number
				if (result == 0 && strcmp(substr,"0") != 0){		//check if the value is a number
					if (debug_indicator == 1){
						printf("***Syntax Error: One of the arguments is not a number\n");		//if it is not a number, print an error message
					}
					else {
						printf("***Syntax Error\n");
					}
					return 0;
				}
				if (i == 1){		//if the imaginary number is in the first position, implies that real part is zero
					real1 = 0;
					imag1 = result;
					first_imag = 1;		//indicate that the first imaginary number has no real part
				}
				else if (i == 2 && first_imag == 1){		//if the imaginary number is in the second position, and the first imaginary number has no real part
					imag2 = result;							//this must be part of the second complex number
					real2 = 0;								//implies that real part of the second complex number is zero
				}
				else if(i == 2){					//if the imaginary number is in the second position, and the first imaginary number has a real part
					if (ArgNum == 3){				//if only two numbers are inputed
						imag2 = result;				//then this must be part of the second complex number
						real2 = 0;					//and the real part must be zero
					}
					else{							//else
						imag1 = result;				//this must be part of the first complex number
					}
				}
				else if (first_imag == 1 && i==3){	//if it is in the third position, and the first imaginary number has no real part
					imag2 = result;					//this must be part of the second complex number
				}
				else if (first_real == 1 && i==3){	//if this is in the third position, and the first complex number was fully real
					imag2 = result;					//this must be part of the second complex number
				}
				else if (i==3){						//if it is in the third position, and the first complex number was real and imaginary
					imag2 = result;					//this must be part of the second complex number
					real2 = 0;						//and the real part must be zero
				}
				else if (i == 4){					//if it is in the fourth position, the two complex numbers must have real and imaginary components
					imag2 = result;					//this must be part of the second complex number
				}
			}
			else{
				real_count++;			//increment the counter for the number of real numbers encountered
				if (real_count > 2 || (real_count == 2 && first_imag == 1)){		//if more than two real numbers are detected or if input is eg '3i 2 4'
					if (debug_indicator == 1){
						printf("***Syntax Error: More than one real number detected or positioned in invalid place\n");		//print an error message
					}
					else {
						printf("***Syntax Error\n");
					}
					return 0;
				}
				result = atof(temp); //change string to floating point number
				if (result == 0 && strcmp(temp,"0") != 0){		//check if the value is a number
					if (debug_indicator == 1){
						printf("***Syntax Error: One of the arguments is not a number\n");		//if it is not a number, print an error message
					}
					else {
						printf("***Syntax Error\n");
					}
					return 0;
				}
				if (i == 1){					//if it is in the first position
					real1 = result;				//this must be part of the first complex number
				}
				else if (i == 2 && first_imag == 1){		//if the second argument is real and the first argument was imaginary
					real2 = result;					//this must be part of the second complex number
					imag_real_count = 1;			//if the input is imaginary real (eg 2i 3), set this sentinel value
				}
				else if(i == 2){					//if the second argument is real, and the first argument was real
					real2 = result;					//this must be part of the first complex number
					imag1 = 0;						//the imaginary component of the first complex number must be zero
					first_real = 1;					//indicate that the first complex number is only real
				}
				else if (i == 3 && i == ArgNum){	//if it is in the third position and it is the last argument
					real2 = result;					//this must be part of the second complex number
					imag2 = 0;						//there will be no complex component then
				}
				else if (i==3){						//if the real number is the third argument, it must be part of the second complex number
					real2 = result;					//this must be part of the second complex number
				}
			}
		}
		if (ArgNum == 3 && real_count == 1 && imag_count == 1){			//if 2 arguments are input, one is real and the other is imaginary
			int imaginary = 0;										//will contain position of the imaginary number
			int real = 0;								//will contain position of the real number
			for(int k = 1; k != 3; k++){
				sprintf(temp, "%s", (*ArgStrings)[k]);			//copy string to another char array
				size = strlen(temp);				//find the size
				if (temp[size-1] == 'i'){				//if the last element is an i, then it is a imaginary number
					imaginary = k;			//store position of imaginary number
				}
				else{
					real = k;			//store position of real number
				}
			}
			if (real == 1 && imaginary == 2){		//if the real number is in the first position and the imaginary in the second
				imag1 = 0;			//then the imaginary component of the first complex number must be zero
				real2 = 0;			//then the real component of the second complex number must be zero				i.e. input: 2 3i -> number1 = 2, number2 = 3i
			}
		}
		printf("Num1: %.2f + %.2f i\n", real1, imag1);			//print results
		printf("Num2: %.2f + %.2f i\n", real2, imag2);			//print results
		double real_result, imag_result;						//will store the real and imaginary result components
		real_result = real1*real2-imag1*imag2;					//find real part
		imag_result = imag1*real2 + real1*imag2;				//find imaginary part
		printf("Result: %.2f + %.2f i\n", real_result, imag_result);			//print result
		return 1;
	}
	if (ArgNum < 3){		//checks if there are less than 3 arguments (including the command argument)
		if (debug_indicator == 1){
			printf("***Syntax Error: Need at least 2 arguments (excluding command)\n");		//if false, then print error
		}
		else{
			printf("***Syntax Error\n");
		}
		return 0;
	}
	for (int i = 1; i< ArgNum; i++){
		if (strcmp("ans", (*ArgStrings)[i]) == 0){			//if the argument is using the previous answer
			num = ans;
		}
		else if (strcmp("PI", (*ArgStrings)[i]) == 0){			//if the argument is using PI
			num = PI;
		}
		else{
			num = atof((*ArgStrings)[i]);		//convert string to floating point number
			if (num == 0 && strcmp((*ArgStrings)[i],"0") != 0){		//check if it is a number
				if (debug_indicator == 1){
					printf("***Syntax Error: One of the arguments is not a number\n");		//if not print error
				}
				else{
					printf("***Syntax Error\n");
				}
				return 0;
			}
		}
		result = result * num;		//multiply it by the result
	}
	printf("   Result: %f\n", result);		//print result
	ans = result;		//store answer
	return 1;
}
int DivFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	double result, num1, num2;			//variables to store result and arguments
	printf("-->Dividing...\n");
	if (complex_mode == 1){			//if complex mode is on
		if (ArgNum < 3){				//function accepts minimum 2 arguments (excluding command)
			if (debug_indicator == 1){
				printf("***Syntax Error: Need at least 2 arguments (excluding command)\n");		//if false, then print error
			}
			else{
				printf("***Syntax Error\n");
			}
			return 0;
		}
		else if (ArgNum > 5){			//function accepts maximum 4 arguments (Excluding command)
			if (debug_indicator == 1){
				printf("***Syntax Error: Need at most 4 arguments\n");		//if false, then print error
			}
			else{
				printf("***Syntax Error\n");
			}
			return 0;
		}

		int size;		//contain size of a string
		int first_imag = 0, first_real = 0;		//indicates whether the imaginary/real number found was the first of its respective type
		int real_count = 0, imag_count = 0,imag_real_count = 0;		//counts the number of occurrences of real/imaginary numbers, and imag_real_count indicates if the input format is of the form "imaginary real number"
		char temp[100], substr[100];			//temporary variable to store an argument, and substring
		double real1, imag1, real2, imag2;		//contains the real/imaginary components of the two complex numbers
		for(int i = 1; i < ArgNum; i++){
			sprintf(temp, "%s", (*ArgStrings)[i]);		//store argument in temporary string
			size = strlen(temp);		//find size of string
			if (temp[size-1] == 'i'){			//if the last element is an i, then it is a imaginary number
				imag_count++;				//increment the counter for the number of imaginary numbers encountered
				if (imag_count > 2){			//if more than two imaginary numbers are found
					if (debug_indicator == 1){
						printf("***Syntax Error: More than one imaginary number detected\n");		//print an error message
					}
					else {
						printf("***Syntax Error\n");
					}
					return 0;
				}
				strncpy(substr, temp, size-1);			//copy the elements excluding the i to another string
				result = atof(substr); //change string to floating point number
				if (result == 0 && strcmp(substr,"0") != 0){		//check if the value is a number
					if (debug_indicator == 1){
						printf("***Syntax Error: One of the arguments is not a number\n");		//if it is not a number, print an error message
					}
					else {
						printf("***Syntax Error\n");
					}
					return 0;
				}
				if (i == 1){		//if the imaginary number is in the first position, implies that real part is zero
					real1 = 0;
					imag1 = result;		//this must be part of the first complex number
					first_imag = 1;		//indicate that the first imaginary number has no real part
				}
				else if (i == 2 && first_imag == 1){		//if the imaginary number is in the second position, and the first imaginary number has no real part
					imag2 = result;							//this must be part of the second complex number
					real2 = 0;								//implies that real part of the second complex number is zero
				}
				else if(i == 2){					//if the imaginary number is in the second position, and the first imaginary number has a real part
					if (ArgNum == 3){				//if only two numbers are inputed
						imag2 = result;				//then this must be part of the second complex number
						real2 = 0;					//and the real part must be zero
					}
					else{							//else
						imag1 = result;				//this must be part of the first complex number
					}
				}
				else if (first_imag == 1 && i==3){	//if it is in the third position, and the first imaginary number has no real part
					imag2 = result;					//this must be part of the second complex number
				}
				else if (first_real == 1 && i==3){	//if this is in the third position, and the first complex number was fully real
					imag2 = result;					//this must be part of the second complex number
				}
				else if (i==3){						//if it is in the third position, and the first complex number was real and imaginary
					imag2 = result;					//this must be part of the second complex number
					real2 = 0;						//and the real part must be zero
				}
				else if (i == 4){					//if it is in the fourth position, the two complex numbers must have real and imaginary components
					imag2 = result;					//this must be part of the second complex number
				}
			}
			else{
				real_count++;			//increment the counter for the number of real numbers encountered
				if (real_count > 2 || (real_count == 2 && first_imag == 1)){		//if more than two real numbers are detected or if input is eg '3i 2 4'
					if (debug_indicator == 1){
						printf("***Syntax Error: More than one real number detected or positioned in invalid place\n");		//print an error message
					}
					else {
						printf("***Syntax Error\n");
					}
					return 0;
				}
				result = atof(temp); //change string to floating point number
				if (result == 0 && strcmp(temp,"0") != 0){		//check if the value is a number
					if (debug_indicator == 1){
						printf("***Syntax Error: One of the arguments is not a number\n");		//if it is not a number, print an error message
					}
					else {
						printf("***Syntax Error\n");
					}
					return 0;
				}
				if (i == 1){					//if it is in the first position
					real1 = result;				//this must be part of the first complex number
				}
				else if (i == 2 && first_imag == 1){		//if the second argument is real and the first argument was imaginary
					real2 = result;					//this must be part of the second complex number
					imag_real_count = 1;			//if the input is imaginary real (eg 2i 3), set this sentinel value
				}
				else if(i == 2){					//if the second argument is real, and the first argument was real
					real2 = result;					//this must be part of the first complex number
					imag1 = 0;						//the imaginary component of the first complex number must be zero
					first_real = 1;					//indicate that the first complex number is only real
				}
				else if (i == 3 && i == ArgNum){	//if it is in the third position and it is the last argument
					real2 = result;					//this must be part of the second complex number
					imag2 = 0;						//there will be no complex component then
				}
				else if (i==3){						//if the real number is the third argument, it must be part of the second complex number
					real2 = result;					//this must be part of the second complex number
				}
			}
		}
		if (ArgNum == 3 && real_count == 1 && imag_count == 1){		//if 2 arguments are input, one is real and the other is imaginary
			int imaginary = 0;										//will contain position of the imaginary number
			int real = 0;								//will contain position of the real number
			for(int k = 1; k != 3; k++){
				sprintf(temp, "%s", (*ArgStrings)[k]);			//copy string to another char array
				size = strlen(temp);				//find the size
				if (temp[size-1] == 'i'){				//if the last element is an i, then it is a imaginary number
					imaginary = k;			//store position of imaginary number
				}
				else{
					real = k;			//store position of real number
				}
			}
			if (real == 1 && imaginary == 2){		//if the real number is in the first position and the imaginary in the second
				imag1 = 0;			//then the imaginary component of the first complex number must be zero
				real2 = 0;			//then the real component of the second complex number must be zero				i.e. input: 2 3i -> number1 = 2, number2 = 3i
			}
		}
		printf("Num1: %.2f + %.2f i\n", real1, imag1);			//print result
		printf("Num2: %.2f + %.2f i\n", real2, imag2);			//print result
		double real_result, imag_result;
		real_result = (real1*real2 + imag1*imag2)/(real2*real2+imag2*imag2);	//calculate real part
		imag_result = (imag1*real2 - real1*imag2)/(real2*real2+imag2*imag2);	//calculate imaginary part
		printf("Result: %.2f + %.2f i\n", real_result, imag_result);			//print result
		return 1;
	} //if complex mode is off
	if (ArgNum != 3){		//this function accepts two arguments excluding command
		if (debug_indicator == 1){
			printf("***Syntax Error: Only 2 arguments allowed (excluding command)\n");
		}
		else{
			printf("***Syntax Error\n");
		}
		return 0;
	}
	if (strcmp("ans", (*ArgStrings)[1]) == 0){			//if the argument is using the previous answer
		num1 = ans;
	}
	else if (strcmp("PI", (*ArgStrings)[1]) == 0){			//if the argument is using PI
		num1 = PI;
	}
	else{
		num1 = atof((*ArgStrings)[1]);			//convert string to float
		if (num1 == 0 && strcmp((*ArgStrings)[1],"0") != 0){			//check if it is a number
			if (debug_indicator == 1){
				printf("***Syntax Error: Argument 1 is not a number\n");
			}
			else{
				printf("***Syntax Error\n");
			}
			return 0;
		}
	}
	if (strcmp("ans", (*ArgStrings)[2]) == 0){				//if the argument is using the previous answer
		num2 = ans;
	}
	else if (strcmp("PI", (*ArgStrings)[2]) == 0){			//if the argument is using PI
		num2 = PI;
	}
	else{
		num2 = atof((*ArgStrings)[2]);			//convert string to float
		if (num2 == 0 && strcmp((*ArgStrings)[2],"0") != 0){			//check if it is a number
			if (debug_indicator == 1){
				printf("***Syntax Error: Argument 2 is not a number\n");			//if not print error
			}
			else{
				printf("***Syntax Error\n");
			}
			return 0;
		}
	}
	if (num2 == 0){			//if the second argument is zero
		if (debug_indicator == 1){
			printf("***Math Error: Cannot divide by zero\n");			//print error because you can't divide by zero
		}
		else{
			printf("***Math Error\n");
		}
		return 0;
	}
	result = num1/num2;			//calculate division
	printf("   Result: %f\n", result);		//print result
	ans = result;		//store answer
	return 1;
}
//radians!
int CosFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	double result, num1;
	printf("-->Cosine...\n");
	if (ArgNum != 2){						//this functions only accepts 1 argument (excluding the command argument) of a decimal
		if (debug_indicator == 1){
			printf("***Syntax Error: Only 1 argument allowed (excluding command argument)\n");
		}
		else{
			printf("***Syntax Error\n");
		}
		return 0;
	}
	if (strcmp("ans", (*ArgStrings)[1]) == 0){				//if the argument is using the previous answer
		num1 = ans;
	}
	else if (strcmp("PI", (*ArgStrings)[1]) == 0){			//if the argument is using PI
		num1 = PI;
	}
	else{
		num1 = atof((*ArgStrings)[1]);		//convert string to float
		if (num1 == 0 && strcmp((*ArgStrings)[1],"0") != 0){		//check if it is a number
			if (debug_indicator == 1){
				printf("***Syntax Error: Argument 1 is not a number\n");		//print error if it is not a number
			}
			else {
				printf("***Syntax Error\n");
			}
			return 0;
		}
	}
	result = cos(num1);		//calculate result
	printf("   Result: %f\n", result);		//print result
	ans = result;		//store answer
	return 1;
}
int SinFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	double result, num1;
	printf("-->Sine...\n");
	if (ArgNum != 2){							//this functions only accepts 1 argument (excluding the command argument) of a decimal
		if (debug_indicator == 1){
			printf("***Syntax Error: Only 1 argument allowed (excluding command argument)\n");		//print error
		}
		else {
			printf("***Syntax Error\n");
		}
		return 0;
	}
	if (strcmp("ans", (*ArgStrings)[1]) == 0){				//if the argument is using the previous answer
		num1 = ans;
	}
	else if (strcmp("PI", (*ArgStrings)[1]) == 0){		//if the argument is using PI
		num1 = PI;
	}
	else{
		num1 = atof((*ArgStrings)[1]);  //convert string to float
		if (num1 == 0 && strcmp((*ArgStrings)[1],"0") != 0){		//check if it is a number
			if(debug_indicator == 1){
				printf("***Syntax Error: Argument 1 is not a number\n");		//print error if not
			}
			else{
				printf("***Syntax Error\n");
			}
			return 0;
		}
	}
	result = sin(num1);		//calculate sine result
	printf("   Result: %f\n", result);		//print result
	ans = result;		//store answer
	return 1;
}
int ExpFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	double result, num1;
	printf("-->Exponential...\n");
	if (ArgNum != 2){						//this functions only accepts 1 argument (excluding the command argument) of a decimal
		if (debug_indicator == 1){
			printf("***Syntax Error: Only 1 argument allowed (excluding command argument)\n");	//print error
		}
		else{
			printf("***Syntax Error\n");
		}
		return 0;
	}
	if (strcmp("ans", (*ArgStrings)[1]) == 0){				//if the argument is using the previous answer
		num1 = ans;
	}
	else if (strcmp("PI", (*ArgStrings)[1]) == 0){			//if the argument is using PI
		num1 = PI;
	}
	else{
		num1 = atof((*ArgStrings)[1]);			//convert string to float
		if (num1 == 0 && strcmp((*ArgStrings)[1],"0") != 0){			//check if it is a number
			if (debug_indicator == 1){
				printf("***Syntax Error: Argument 1 is not a number\n");		//if not print error
			}
			else{
				printf("***Syntax Error\n");
			}
			return 0;
		}
	}
	result = exp(num1);		//find exponential of number
	printf("   Result: %f\n", result);		//print result
	ans = result;		//store answer
	return 1;
}
int LnFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	double result, num1;
	printf("-->Natural logarithm...\n");
	if (ArgNum != 2){						//this functions only accepts 1 argument (excluding the command argument) of a decimal
		if (debug_indicator == 1){
			printf("***Syntax Error: Only 1 argument allowed (excluding command argument)\n");		//print error
		}
		else{
			printf("***Syntax Error\n");
		}
		return 0;
	}
	if (strcmp("ans", (*ArgStrings)[1]) == 0){			//if the argument is using the previous answer
		num1 = ans;
	}
	else if (strcmp("PI", (*ArgStrings)[1]) == 0){			//if the argument is using PI
		num1 = PI;
	}
	else{
		num1 = atof((*ArgStrings)[1]);			//convert string to float
		if (num1 == 0 && strcmp((*ArgStrings)[1],"0") != 0){		//check if it is a number
			if(debug_indicator == 1){
				printf("***Syntax Error: Argument 1 is not a number\n");		//if not print error
			}
			else {
				printf("***Syntax Error\n");
			}
			return 0;
		}
	}
	if (num1 <= 0){			//if the first argument is negative or zero (natural logarithm cannot accept negative or zero numbers)
		if (debug_indicator == 1){
			printf("***Math Error: Zero or negative numbers cannot be performed with the natural logarithm\n", num1);  //print error
		}
		else {
			printf("***Math Error\n");
		}
		return 0;
	}
	result = log(num1);		//find log of number
	printf("   Result: %f\n", result);		//print result
	ans = result;		//store answer
	return 1;
}
int LogFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	double result, num1;
	printf("-->Log Base 10...\n");
	if (ArgNum != 2){						//this functions only accepts 1 argument (excluding the command argument) of a decimal
		if (debug_indicator == 1){
			printf("***Syntax Error: Only 1 argument allowed\n");		//print error
		}
		else{
			printf("***Syntax Error\n");
		}
		return 0;
	}
	if (strcmp("ans", (*ArgStrings)[1]) == 0){			//if the argument is using the previous answer
		num1 = ans;
	}
	else if (strcmp("PI", (*ArgStrings)[1]) == 0){			//if the argument is using PI
		num1 = PI;
	}
	else{
		num1 = atof((*ArgStrings)[1]);			//convert string to float
		if (num1 == 0 && strcmp((*ArgStrings)[1],"0") != 0){			//check if it is a number
			if (debug_indicator == 1){
				printf("***Syntax Error: Argument 1 is not a number\n");			//print error if not
			}
			else{
				printf("***Syntax Error\n");
			}
			return 0;
		}
	}
	if (num1 <= 0){			//if the first argument is negative or zero (logarithm cannot accept negative or zero numbers)
		if(debug_indicator == 1){
			printf("***Math Error: Zero or negative numbers cannot be performed with base 10 logarithm\n", num1);		//print error
		}
		else{
			printf("***Math Error\n");
		}
		return 0;
	}
	result = log10(num1);			//calculate result
	printf("   Result: %f\n", result);		//print result
	ans = result;		//store answer
	return 1;
}
int PowFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	double result, num1, num2;
	printf("-->Power (x^y)...\n");
	if (ArgNum != 3){					//this functions only accepts 2 arguments (excluding the command argument) of two decimal
		if(debug_indicator == 1){
			printf("***Syntax Error: Only 2 arguments allowed (excluding command argument)\n");			//print error
		}
		else{
			printf("***Syntax Error\n");
		}
		return 0;
	}
	if (strcmp("ans", (*ArgStrings)[1]) == 0){				//if the argument is using the previous answer
		num1 = ans;
	}
	else if (strcmp("PI", (*ArgStrings)[1]) == 0){			//if the argument is using PI
		num1 = PI;
	}
	else{
		num1 = atof((*ArgStrings)[1]);			//convert string to float
		if (num1 == 0 && strcmp((*ArgStrings)[1],"0") != 0){			//check if it is a number
			if(debug_indicator == 1){
				printf("***Syntax Error: Argument 1 is not a number\n");			//print error
			}
			else{
				printf("***Syntax Error\n");
			}
			return 0;
		}
	}
	if (strcmp("ans", (*ArgStrings)[2]) == 0){						//if the argument is using the previous answer
		num2 = ans;
	}
	else if (strcmp("PI", (*ArgStrings)[2]) == 0){			//if the argument is using PI
		num2 = PI;
	}
	else{
		num2 = atof((*ArgStrings)[2]);			//convert string to float
		if (num2 == 0 && strcmp((*ArgStrings)[2],"0") != 0){			//check if it is a number
			if(debug_indicator == 1){
				printf("***Syntax Error: Argument 2 is not a number\n");		//if not, print error
			}
			else{
				printf("***Syntax Error\n");
			}
			return 0;
		}
	}

	result = pow(num1,num2);				//calculate num1^num2
	printf("   Result: %f\n", result);		//print result
	ans = result;			//store answer
	return 1;
}
int SqrtFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	double result, num1;
	printf("-->Square root...\n");
	if (ArgNum != 2){						//this functions only accepts 1 argument (excluding the command argument) of a decimal
		if(debug_indicator == 1){
			printf("***Syntax Error: Only 1 argument allowed (excluding command argument)\n");			//else print error
		}
		else{
			printf("***Syntax Error\n");
		}
		return 0;
	}
	if (strcmp("ans", (*ArgStrings)[1]) == 0){				//if the argument is using the previous answer
		num1 = ans;
	}
	else if (strcmp("PI", (*ArgStrings)[1]) == 0){			//if the argument is using PI
		num1 = PI;
	}
	else{
		num1 = atof((*ArgStrings)[1]);			//convert string to float
		if (num1 == 0 && strcmp((*ArgStrings)[1],"0") != 0){		//check if it is a number
			if(debug_indicator == 1){
				printf("***Syntax Error: Argument 1 is not a number\n");			//if not print error
			}
			else{
				printf("***Syntax Error\n");
			}
			return 0;
		}
	}
	if (num1 < 0){			//if it is a negative number
		result = sqrt(num1*-1);			//find the square root of the magnitude of the number
		printf("   Result: %f i (complex)\n", result);			//print as an imaginary number
	}
	else{			//else if it is a positive number
		result = sqrt(num1);			//calculate the square root
		printf("   Result: %f\n", result);			//print result
	}
	ans = result;		//save answer
	return 1;
}
int FactorialFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	int result = 1;
	int num1;
	printf("-->Factorial...\n");
	if (ArgNum != 2){						//this functions only accepts 1 argument (excluding the command argument) of an integer
		if(debug_indicator == 1){
			printf("***Syntax Error: Only 1 argument allowed (excluding command argument)\n");
		}
		else{
			printf("***Syntax Error\n");
		}
		return 0;
	}
	if (strcmp("ans", (*ArgStrings)[1]) == 0){					//if the argument is using the previous answer
		num1 = ans;
	}
	else if (strcmp("PI", (*ArgStrings)[1]) == 0){			//if the argument is using PI
		num1 = PI;
	}
	else{
		num1 = atoi((*ArgStrings)[1]);			//convert string to integer
		if (num1 == 0 && strcmp((*ArgStrings)[1],"0") != 0){			//check if it is a number
			if(debug_indicator == 1){
				printf("***Syntax Error: Argument 1 is not a number\n");
			}
			else{
				printf("***Syntax Error\n");
			}
			return 0;
		}
	}
	int integer_num = num1;
	double floating_num = num1;
	if (num1 < 0){		//if it is a negative number
		if(debug_indicator == 1){
			printf("***Math Error: Factorial only accepts positive numbers\n");		//print error (no negative numbers accepted)
		}
		else{
			printf("***Math Error\n");
		}
		return 0;
	}
	else if (floating_num = num1){		//if the number is a floating number
		if(debug_indicator == 1){
			printf("***Math Error: Factorial does not accept floating numbers\n");	//print error
		}
		else{
			printf("***Math Error\n");
		}
		return 0;
	}
	else if (integer_num = num1){			//else if it is an integer
		//factorial doesn't work after 12
		for(int i = 1; i<=num1; i++){
			result = result * i;			//calculate the factorial
		}
		printf("   Result: %d\n", result);		//print result
	}
	ans = result;			//store answer
	return 1;
}
int DebugFunction(unsigned int ArgNum,uint8_t **ArgStrings[])
{
	//debug
	if (ArgNum != 2){					//this function only accepts 1 argument of 'on' or 'off'
		if (debug_indicator == 1){
			printf("***Syntax Error: debug function requires 1 argument (excluding command argument)\n");
		}
		else{
			printf("***Syntax Error\n");
		}
		return 0;
	}
	if (strcmp("on",(*ArgStrings)[1])==0){			//if the argument is 'on'
		if (debug_indicator == 1){
			printf("Turning debug on\n");
		}
		debug_indicator = 1;			//turn on debug
	}
	else if (strcmp("off",(*ArgStrings)[1])==0){	//if the argument is 'off'
		if(debug_indicator == 1){
			printf("Turning debug off\n");
		}
		debug_indicator = 0;			//turn off debug
	}
	else {
		if(debug_indicator == 1){			//if the argument is not on or off
			printf("***Syntax Error: second argument should be 'on' or 'off'\n");		//print error
		}
		else{
			printf("***Syntax Error\n");
		}
		return 0;
	}
	return 1;
}
int ComplexFunction(unsigned int ArgNum,uint8_t **ArgStrings[])
{
	if (ArgNum != 2){				//this functions only accepts 1 argument of 'on' or 'off'
		if (debug_indicator == 1){
			printf("***Syntax Error: complex mode function requires 1 argument (excluding the command argument)\n");		//print error
		}
		else{
			printf("***Syntax Error\n");
		}
		return 0;
	}
	if (strcmp("on",(*ArgStrings)[1])==0){		//if the argument is 'on'
		if (complex_mode == 1){
			printf("Turning complex mode on\n");
		}
		complex_mode = 1;			//turn on complex mode
	}
	else if (strcmp("off",(*ArgStrings)[1])==0){			//if the argument is 'off'
		if(complex_mode == 1){
			printf("Turning complex mode off\n");
		}
		complex_mode = 0;			//turn off complex mode
	}
	else {
		if(debug_indicator == 1){		//if the argument is not on or off
			printf("***Syntax Error: second argument should be 'on' or 'off'\n");			//print error
		}
		else{
			printf("***Syntax Error\n");
		}
		return 0;
	}
	return 1;
}
int HelpFunction(unsigned int ArgNum,uint8_t **ArgStrings[])
{
	const command_s Commandlist[] = {
			{"add", &AddFunction, "add <num 1> .. <num N>. For complex: numbers can be real (x) or imaginary (yi)"},
			{"sub", &SubFunction, "sub <num 1> <num 2>. For complex: numbers can be real (x) or imaginary (yi)"},
			{"mul", &MulFunction, "mul <num 1> .. <num N>. For complex: only two complex numbers allowed: mul <real1> <imaginary1> <real2> <imaginary2>"},
			{"div", &DivFunction, "div <num 1> <num 2>. For complex: only two complex numbers allow: div <real1> <imaginary1> <real2> <imaginary2>"},
			{"cos", &CosFunction, "cos <num 1>"},
			{"sin", &SinFunction, "sin <num 1>"},
			{"exp", &ExpFunction, "exp <num 1>"},
			{"ln", &LnFunction, "ln <num 1>"},
			{"log", &LogFunction, "log <num 1>"},
			{"pow", &PowFunction, "pow <num 1>"},
			{"sqrt", &SqrtFunction, "sqrt <num 1>"},
			{"fact", &FactorialFunction, "fact <num 1>"},
			{"sinh", &SinhFunction, "sinh <num 1>"},
			{"cosh", &CoshFunction, "cosh <num 1>"},
			{"tanh", &TanhFunction, "tanh <num 1>"},
			{"convert", &ConvertFunction, "convert <type (hex, oct, bin, dec)> <number>"},
			{"help", &HelpFunction, "Turn debug messages on or off"},
			{"debug", &DebugFunction, "Display help messages"},
			{"complex", &ComplexFunction, "Switch complex mode on/off"},
			{"and", &AndFunction, "and <type (hex, oct, bin, dec) <num 1> <num 2>"},
			{"or", &OrFunction, "or <type (hex, oct, bin, dec) <num 1> <num 2>"},
			{"xor", &XorFunction, "xor <type (hex, oct, bin, dec) <num 1> <num 2>"},
			{"comp", &ComplementFunction, "comp <type (hex, oct, bin, dec) <num 1>"},
			{"real", &RealFunction, "Complex mode only: real <real part> <imaginary part>"},
			{"imag", &ImaginaryFunction, "Complex mode only: imag <real part> <imaginary part>"},
			{"polar", &PolarFunction, "Complex mode only: polar <real part> <imaginary part>"},
			{NULL, NULL, NULL}
	};
	for (int k = 0; k < (FUNC_NUM - 1); k++){		//cycle through the structure for the command
		if (strcmp((*ArgStrings)[1], Commandlist[k].NameString)==0){		//if the command is found
			printf("%s: %s\n", Commandlist[k].NameString, Commandlist[k].HelpString);		//print the helpstring corresponding to the command
			break;
		}
	}
	if (strcmp((*ArgStrings)[1], "help") == 0)		//if the command was help
	{
		for (int k = 0; k < FUNC_NUM; k++)
		{
			printf("%s: %s\n", Commandlist[k].NameString, Commandlist[k].HelpString); //print the help strings of all the commands
		}
	}
	return 1;
}
int SinhFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	double result, num1;
	printf("-->Hyperbolic Sine...\n");
	if (ArgNum != 2){							//this functions only accepts 1 argument (excluding the command argument) of a decimal in radians
		if (debug_indicator == 1){
			printf("***Syntax Error: Only 1 argument allowed\n");			//print error
		}
		else {
			printf("***Syntax Error\n");
		}
		return 0;
	}
	if (strcmp("ans", (*ArgStrings)[1]) == 0){					//if the argument is using the previous answer
		num1 = ans;
	}
	else if (strcmp("PI", (*ArgStrings)[1]) == 0){				//if the argument is using PI
		num1 = PI;
	}
	else{
		num1 = atof((*ArgStrings)[1]);				//convert string to float
		if (num1 == 0 && strcmp((*ArgStrings)[1],"0") != 0){		//check if it is a number
			if(debug_indicator == 1){
				printf("***Syntax Error: Argument 1 is not a number\n");		//print error
			}
			else{
				printf("***Syntax Error\n");
			}
			return 0;
		}
	}
	result = sinh(num1);			//calculate sinh
	printf("   Result: %f\n", result);			//print result
	ans = result;			//store answer
	return 1;
}
int CoshFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	double result, num1;
	printf("-->Hyperbolic Cos...\n");
	if (ArgNum != 2){							//this functions only accepts 1 argument (excluding the command argument) of a decimal in radians
		if (debug_indicator == 1){
			printf("***Syntax Error: Only 1 argument allowed\n");		//print error
		}
		else {
			printf("***Syntax Error\n");
		}
		return 0;
	}
	if (strcmp("ans", (*ArgStrings)[1]) == 0){			//if the argument is using the previous answer
		num1 = ans;
	}
	else if (strcmp("PI", (*ArgStrings)[1]) == 0){			//if the argument is using PI
		num1 = PI;
	}
	else{
		num1 = atof((*ArgStrings)[1]);				//convert string to float
		if (num1 == 0 && strcmp((*ArgStrings)[1],"0") != 0){
			if(debug_indicator == 1){
				printf("***Syntax Error: Argument 1 is not a number\n");	//print error
			}
			else{
				printf("***Syntax Error\n");
			}
			return 0;
		}
	}
	result = cosh(num1);		//calculate result
	printf("   Result: %f\n", result);		//print result
	ans = result;		//store answer
	return 1;
}
int TanhFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	double result, num1;
	printf("-->Hyperbolic Tan...\n");
	if (ArgNum != 2){						//this functions only accepts 1 argument (excluding the command argument) of a decimal in radians
		if (debug_indicator == 1){
			printf("***Syntax Error: Only 1 argument allowed (excluding command argument)\n");		//print error
		}
		else {
			printf("***Syntax Error\n");
		}
		return 0;
	}
	if (strcmp("ans", (*ArgStrings)[1]) == 0){			//if the argument is using the previous answer
		num1 = ans;
	}
	else if (strcmp("PI", (*ArgStrings)[1]) == 0){			//if the argument is using PI
		num1 = PI;
	}
	else{
		num1 = atof((*ArgStrings)[1]);				//convert string to float
		if (num1 == 0 && strcmp((*ArgStrings)[1],"0") != 0){		//check if it is a number
			if(debug_indicator == 1){
				printf("***Syntax Error: Argument 1 is not a number\n");		//print error
			}
			else{
				printf("***Syntax Error\n");
			}
			return 0;
		}
	}
	result = tanh(num1);				//calculat hyperbolic tan of number
	printf("   Result: %f\n", result);		//print error
	ans = result;		//store answer
	return 1;
}
int ConvertFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	int result;
	printf("-->Converting...\n");
	char argument[100];
	char type[100];
	if (ArgNum != 3){					//this functions only accepts two arguments (excluding the command argument) of variable type and one value of that type
		if (debug_indicator == 1){
			printf("***Syntax Error: Only 2 argument allowed (excluding command argument)\n");		//print error
		}
		else {
			printf("***Syntax Error\n");
		}
		return 0;
	}
	sscanf((*ArgStrings)[1],"%s", type);		//scan string into temporary variable
	sscanf((*ArgStrings)[2],"%s", argument);
	if (NumberSystemCheck(&type, &argument)){				//if the argument is valid (found using the NumberSystemCheck function)
		if (strcmp("hex", (*ArgStrings)[1]) == 0){
			result = (int) strtol((*ArgStrings)[2], NULL, HEX_BASE);			//convert string to long int
		}
		else if (strcmp("bin", (*ArgStrings)[1]) == 0){							//printf at the end, calculate binary at end using result
			result = (int) strtol((*ArgStrings)[2], NULL, BINARY_BASE);			//convert string to long int
		}
		else if (strcmp("dec", (*ArgStrings)[1]) == 0){
			result = (int) strtol((*ArgStrings)[2],NULL, DECIMAL_BASE);			//convert string to long int
		}
		else if (strcmp("oct", (*ArgStrings)[1]) == 0){
			result = (int) strtol((*ArgStrings)[2], NULL, OCTAL_BASE);			//convert string to long int
		}
		print_result(result);		//print result using function
	}
	else{
		return 0;
	}
	return 1;
}
void print_result(int result){			//function to print result as decimal, hexdecimal, binary, octal
	int remainder, n = fabs(result);
	long binary = 0, i = 1;
	while(n!=0){		//to convert decimal to binary format
		remainder = n%2;		//find the remainder
		n = n/2;			//integer divide the integer value
		binary = binary + (remainder*i);
		i = i*10;
	}
	printf("   Decimal: %d\n", result);
	if (result < 0){		//if it is a negative number, print it as a hexadecimal, octal, binary, and decimal
		result = result*-1;		//change negative number to positive number
		printf("   Hexadecimal: -%X\n", result);
		printf("   Octal: -%o\n", result);
		printf("   Binary: -%ld\n", binary);
	}
	else{			//if it is a positive number
		printf("   Hexadecimal: %X\n", result);
		printf("   Octal: %o\n", result);
		printf("   Binary: %ld\n", binary);
	}
	return;
}
int AndFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	int result, arg1, arg2;
	printf("-->Bitwise AND...\n");
	char argument1[100], argument2[100], type[100];
	if (ArgNum != 4){						//this functions only accepts three arguments (excluding the command argument) of variable type and two values of that type
		if (debug_indicator == 1){
			printf("***Syntax Error: 3 argument required (excluding the command argument)\n");		//print error
		}
		else {
			printf("***Syntax Error\n");
		}
		return 0;
	}
	sscanf((*ArgStrings)[1],"%s", type);			//scan string into temporary variable
	sscanf((*ArgStrings)[2],"%s", argument1);
	sscanf((*ArgStrings)[3],"%s", argument2);
	if (!NumberSystemCheck(&type, &argument1)){			//if the argument is not valid (found using the NumberSystemCheck function)
		return 0;
	}
	else if (!NumberSystemCheck(&type, &argument2)){			//if the argument is not valid (found using the NumberSystemCheck function)
		return 0;
	}
	else{
		if (strcmp("hex", (*ArgStrings)[1]) == 0){
			arg1 = (int) strtol((*ArgStrings)[2], NULL, HEX_BASE);		//convert string to long int
			arg2 = (int) strtol((*ArgStrings)[3], NULL, HEX_BASE);
		}
		else if (strcmp("bin", (*ArgStrings)[1]) == 0){
			arg1 = (int) strtol((*ArgStrings)[2], NULL, BINARY_BASE);		//convert string to long int
			arg2 = (int) strtol((*ArgStrings)[3], NULL, BINARY_BASE);
		}
		else if (strcmp("dec", (*ArgStrings)[1]) == 0){
			arg1 = (int) strtol((*ArgStrings)[2],NULL, DECIMAL_BASE);		//convert string to long int
			arg2 = (int) strtol((*ArgStrings)[3],NULL, DECIMAL_BASE);
		}
		else if (strcmp("oct", (*ArgStrings)[1]) == 0){
			arg1 = (int) strtol((*ArgStrings)[2], NULL, OCTAL_BASE);		//convert string to long int
			arg2 = (int) strtol((*ArgStrings)[3], NULL, OCTAL_BASE);
		}

	}
	result = arg1 & arg2;		//calculate AND result
	print_result(result);		//print result using function
	return 1;
}
int NumberSystemCheck(uint8_t type[], uint8_t argument[]){
	if (strcmp("hex", type) == 0){			//if the input is hexadecimal
		int size = strlen(argument);		//find size of string
		for (int i = 0; i < size; i++){
			if(isxdigit(argument[i]) == 0){		//check each digit to see if it is hexadecimal valid
				if(debug_indicator == 1){
					printf("***Syntax Error: Invalid HEXADECIMAL input \n");		//if not print error
				}
				else{
					printf("***Syntax Error\n");
				}
				return 0;
			}
		}
	}
	else if (strcmp("bin", type) == 0){							//printf at the end, calculate binary at end using result
		int size = strlen(argument);			//find length of string
		for (int i = 0; i < size; i++){			//compare char by chr to check if they are 0 or 1
			if(argument[i] != '0' && argument[i]!='1'){
				if(debug_indicator == 1){
					printf("***Syntax Error: Invalid BINARY input \n");		//if it is an invalid binary input, print error
				}
				else{
					printf("***Syntax Error\n");
				}
				return 0;
			}
		}
	}
	else if (strcmp("dec", type) == 0){
		int num1 = atoi(argument);		//convert from string to integer
		if (num1 == 0 && strcmp(argument,"0") != 0){			//check if it is a number
			if(debug_indicator == 1){
				printf("***Syntax Error: Decimal was chosen, so the argument cannot be a letter\n");		//if not print error
			}
			else{
				printf("***Syntax Error\n");
			}
			return 0;
		}
	}
	else if (strcmp("oct", type) == 0){		//if it is an octal
		int size = strlen(argument);		//find size of string
		for (int i = 0; i < size; i++){		//compare character by character to check if they are octal valid
			if((isdigit(argument[i]) == 0) || (argument[i] == '8') || (argument[i] == '9')){
				if(debug_indicator == 1){
					printf("***Syntax Error: Invalid OCTAL input \n");		//if invalid, print error
				}
				else{
					printf("***Syntax Error\n");
				}
				return 0;
			}
		}
	}
	else{			//if the variable type argument is invalid
		if (debug_indicator == 1){
			printf("***Syntax Error: Argument %s not recognised\n", argument);			//print error
		}
		else{
			printf("***Syntax Error\n");
		}
		return 0;
	}
	return 1;
}
int OrFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	int result, arg1, arg2;
	printf("-->Bitwise AND...\n");
	char argument1[100], argument2[100], type[100];
	if (ArgNum != 4){					//this functions only accepts three arguments (excluding the command argument) of variable type and two values of that type
		if (debug_indicator == 1){
			printf("***Syntax Error: 3 argument required (excluding command argument)\n");
		}
		else {
			printf("***Syntax Error\n");
		}
		return 0;
	}
	sscanf((*ArgStrings)[1],"%s", type);			//scan string into temporary variable
	sscanf((*ArgStrings)[2],"%s", argument1);
	sscanf((*ArgStrings)[3],"%s", argument2);
	if (!NumberSystemCheck(&type, &argument1)){				//if the argument is not valid (found using the NumberSystemCheck function)
		return 0;
	}
	else if (!NumberSystemCheck(&type, &argument2)){			//if the argument is not valid (found using the NumberSystemCheck function)
		return 0;
	}
	else{
		if (strcmp("hex", (*ArgStrings)[1]) == 0){			//check to see if input is a hexadecimal
			arg1 = (int) strtol((*ArgStrings)[2], NULL, HEX_BASE);			//convert string to long int
			arg2 = (int) strtol((*ArgStrings)[3], NULL, HEX_BASE);
		}
		else if (strcmp("bin", (*ArgStrings)[1]) == 0){			//check to see if the input is a binary
			arg1 = (int) strtol((*ArgStrings)[2], NULL, BINARY_BASE);
			arg2 = (int) strtol((*ArgStrings)[3], NULL, BINARY_BASE);
		}
		else if (strcmp("dec", (*ArgStrings)[1]) == 0){		//check to see if the input is a decimal
			arg1 = (int) strtol((*ArgStrings)[2],NULL, DECIMAL_BASE);
			arg2 = (int) strtol((*ArgStrings)[3],NULL, DECIMAL_BASE);
		}
		else if (strcmp("oct", (*ArgStrings)[1]) == 0){			//check to see if the input is an octal
			arg1 = (int) strtol((*ArgStrings)[2], NULL, OCTAL_BASE);
			arg2 = (int) strtol((*ArgStrings)[3], NULL, OCTAL_BASE);
		}

	}
	result = arg1 | arg2;
	print_result(result);
	return 1;
}
int XorFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	int result, arg1, arg2;
	printf("-->Bitwise AND...\n");
	char argument1[100], argument2[100], type[100];
	if (ArgNum != 4){						//this functions only accepts three arguments (excluding the command argument) of variable type and two values of that type
		if (debug_indicator == 1){
			printf("***Syntax Error: 3 argument required (excluding command argument)\n");		//print error
		}
		else {
			printf("***Syntax Error\n");
		}
		return 0;
	}
	sscanf((*ArgStrings)[1],"%s", type);			//scan string into temporary variable
	sscanf((*ArgStrings)[2],"%s", argument1);
	sscanf((*ArgStrings)[3],"%s", argument2);
	if (!NumberSystemCheck(&type, &argument1)){			//if the argument is not valid (found using the NumberSystemCheck function)
		return 0;
	}
	else if (!NumberSystemCheck(&type, &argument2)){		//if the argument is not valid (found using the NumberSystemCheck function)
		return 0;
	}
	else{
		if (strcmp("hex", (*ArgStrings)[1]) == 0){								//if it is hexadecimal
			arg1 = (int) strtol((*ArgStrings)[2], NULL, HEX_BASE);				//convert the string to an int using the hexadecimal base of 16
			arg2 = (int) strtol((*ArgStrings)[3], NULL, HEX_BASE);
		}
		else if (strcmp("bin", (*ArgStrings)[1]) == 0){							//if it is binary
			arg1 = (int) strtol((*ArgStrings)[2], NULL, BINARY_BASE);			//convert the string to an int using the binary base of 2
			arg2 = (int) strtol((*ArgStrings)[3], NULL, BINARY_BASE);
		}
		else if (strcmp("dec", (*ArgStrings)[1]) == 0){							//if it is decimal
			arg1 = (int) strtol((*ArgStrings)[2],NULL, DECIMAL_BASE);			//convert the string to an int using the decimal base of 10
			arg2 = (int) strtol((*ArgStrings)[3],NULL, DECIMAL_BASE);
		}
		else if (strcmp("oct", (*ArgStrings)[1]) == 0){							//if it is octal
			arg1 = (int) strtol((*ArgStrings)[2], NULL, OCTAL_BASE);			//convert the string to an int using the octal base of 8
			arg2 = (int) strtol((*ArgStrings)[3], NULL, OCTAL_BASE);
		}

	}
	result = arg1 ^ arg2;		//xor the two arguments
	print_result(result);			//print the results using the function
	return 1;
}
int ComplementFunction(unsigned int ArgNum,uint8_t **ArgStrings[]){
	int result, arg1;
	printf("-->Bitwise Complement...\n");
	char argument1[100], type[100];
	if (ArgNum != 3){		//the function only accepts two arguments excluding the command argument, of the variable type and one value of that type
		if (debug_indicator == 1){
			printf("***Syntax Error: 2 arguments required (excluding command argument)\n");		//print error
		}
		else {
			printf("***Syntax Error\n");
		}
		return 0;
	}
	sscanf((*ArgStrings)[1],"%s", type);		//scanf argument string into another string for easier handling
	sscanf((*ArgStrings)[2],"%s", argument1);
	if (!NumberSystemCheck(&type, &argument1)){		//if the argument is not valid (found using the NumberSystemCheck function)
		return 0;		//return 0 and exit the function
	}
	else{		//if the input is valid
		if (strcmp("hex", (*ArgStrings)[1]) == 0){		//if it is a hex number
			arg1 = (int) strtol((*ArgStrings)[2], NULL, HEX_BASE);		//convert the string to an int using the hexadecimal base of 16
		}
		else if (strcmp("bin", (*ArgStrings)[1]) == 0){						//if it is a binary number
			arg1 = (int) strtol((*ArgStrings)[2], NULL, BINARY_BASE);			//convert the string to an int using the binary base of 2
		}
		else if (strcmp("dec", (*ArgStrings)[1]) == 0){						//if it is a decimal
			arg1 = (int) strtol((*ArgStrings)[2],NULL, DECIMAL_BASE);		//convert the string to an int using the decimal base of 10
		}
		else if (strcmp("oct", (*ArgStrings)[1]) == 0){						//if it is an octal
			arg1 = (int) strtol((*ArgStrings)[2], NULL, OCTAL_BASE);		//convert the string to an int using the octal base of 8
		}
	}
	result = ~arg1;		//find the complement of the argument
	print_result(result);		//print result using a function
	return 1;
}
