#include "Ass-03.h"

// This is the console task. Can use your code from the previous assignment
// and expand.
//
// The example code is very basic, showing access to the SD card. Note that
// if the SD card is access from both this task and the pulse rate monitor
// task, a MUTEX will be required.
//
// *** MAKE UPDATES TO THE CODE AS REQUIRED ***
//


FIL MyFile;
FIL MyFile2, MyFile3;
FRESULT Status;
#define BUFF_SIZE 256
#define FUNC_NUM 10
#define AEST 10
#define MEM_NO 5

void Ass_03_Task_01(void const * argument)
{
	uint32_t loop=0;
	uint8_t ts[100];
	uint16_t i;
	Coordinate display;
	char c;
	safe_printf("\e[1;1H\e[2J");
	safe_printf("\033[3J");
	safe_printf("Hello from Task 1 - Console (serial input)\n");
	safe_printf("INFO: Initialise LCD and TP first...\n");

	// STEPIEN: Initialize and turn on LCD and calibrate the touch panel
	// Initialise debug mode
	BSP_LCD_Init();
	BSP_LCD_DisplayOn();
	BSP_TP_Init();
	screen_init();
	info_init();
	safe_printf("%sStart\n%s", WHITE, WHITE);

	// Signal other tasks to start
	osSignalSet(myTask02Handle, 1);
	osSignalSet(myTask03Handle, 1);
	osSignalSet(myTask04Handle, 1);

	//Structure containing the command line commands
	const command_s CommandList[] = {
			{"ls", &ls_function, "Lists contents of the current folder", 0},
			{"cd", &cd_function, "Change the folder to <dir>", 0},
			{"mkdir", &mkdir_function, "Create a new folder <dir>", 0},
			{"cp", &cp_function, "copy the file to another destination: cp <scr> <dst>", 0},
			{"rm", &rm_function, "remove the file or folder: rm <scr>", 0},
			{"debug", &debug_function, "change debug", 1},
			{"analog", &speed_function, "set speed", 1},
			{"help", &HelpFunction, "View command list", 0},
			{"mov", &mov_function, "Move source file to destination: mov <scr> <dst>", 0},
			{"memory", &memory_setting_function, "set memory", 1},
			{NULL, NULL, NULL}
	};
	int max_size = 200;	// max size of string
	static char *expression;			//will contain the string that is typed in putty
	expression = (char *) malloc(max_size * sizeof(char));	// malloc
	if(!expression){		//check that memory has been successfully allocated
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: Cannot allocate memory\n%s",RED,WHITE);
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return;
	}
	int position = 0;				//position in the string to add the character read

	while (1)
	{
		c = getchar();			//get a character from PUTTY
		safe_printf("%c", c);	//echo the character back to PUTTY console
		fflush(stdout);
		//HAL_GPIO_TogglePin(GPIOD, LD4_Pin);  // Toggle LED4

		expression[position] = c;			//save the character in an array
		position++;							//increment the position in the array

		// If we get a return character then process the string
		if (c == '\r' || position > 101) {			//if the character was 'enter' or the array is full
			safe_printf("\n");						//print a new line
			expression[position - 1] = '\0';		//save a null character at the end of the string
			char ** array_of_words;		// output array of pointers to the start of each world
			array_of_words = (char * ) malloc (max_size * sizeof(char));		//allocate memory to contain the parsed strings
			if(!array_of_words){	//check that memory has been successfully allocated
				if (get_debug())		// if debug is turned on
				{
					safe_printf("%sERROR: Cannot allocate memory\n%s",RED,WHITE);
				}
				else{
					safe_printf("%sERROR%s\n",RED,WHITE);
				}
				return;
			}
			int count;					//word count
			int j = 0;
			char character_to_search = ' ';		// character that separates two words, in this case a space
			count = string_parser(expression, &array_of_words,character_to_search);		//parse the string
			position = 0;		//reset the position
			expression[position] = '\0';		//add a null character at the beginning of the old string
			if (get_debug())		// if debug is turned on
			{
				safe_printf("Word count = %d\n", count);
			}
			int position = -1;		//sentinel character to indicate position of the command in the array (-1 if command not found)
			for (int k=0; k < FUNC_NUM; k++){			//search through the array of structures
				if(strcmp(array_of_words[0],CommandList[k].NameString)==0){		//compare the current command with the command written on the console
					position = k;			//if they are the same, ie command found, save the current position
					break;			//exit for loop
				}
			}
			if(position == -1){			//if the command was not found, print error message
				if (get_debug()){		//if debug is turned on
					safe_printf("%sERROR: Command not found\n%s", RED,WHITE);
				}
				else{
					safe_printf("%sERROR\n%s",RED,WHITE);
				}
			}
			else if (CommandList[position].Type == 0){			//if the command was found, call the corresponding function
				FRESULT res;
				osMutexWait(myMutex01Handle, osWaitForever);			//protect the shared resource of accessing the SD card
				res = CommandList[position].Function_p(count,&array_of_words);
				osMutexRelease(myMutex01Handle);
			}
			else if (CommandList[position].Type == 1)
			{
				osMutexWait(myMutex01Handle, osWaitForever);
				CommandList[position].Function_p(count,&array_of_words);
				osMutexRelease(myMutex01Handle);
			}
		}
		osDelay(10);
	}
}

int string_parser(char *inp, char ***array_of_words_p, char character_to_search)
{
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
	if(!copy){			//check that memory has been successfully allocated
		safe_printf("%sERROR: Cannot allocate memory\n%s",RED,WHITE);
		return -1;
	}
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
	return no_of_words;		// return the number of words in the string
}
//change the current directory
FRESULT cd_function(unsigned int ArgNum, uint8_t **Arguments[]){
	FRESULT res;
	//******************check for invalid arguments
	if(ArgNum != 2){			//if the number of arguments is not 2
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR 'cd' requires one argument\n%s",RED,WHITE);		//print error; cd needs to be in the format of <cd path>
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return 6;
	}
	if((res = f_chdir(((*Arguments)[1]))) == FR_NO_PATH){			//if the path could not be found, print error
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: Could not find the path\n%s",RED,WHITE);
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return res;
	}
	else if (res == FR_INVALID_NAME){				//if the path name is invalid, print error
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: The path name format is invalid%s\n",RED,WHITE);
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return res;
	}
	else if (res == FR_INVALID_OBJECT){				//if the file/directory object is invalid, print error and exit function
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: The file/directory object is invalid%s\n",RED,WHITE);
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return res;
	}
	else if (res == FR_OK){			//if the directory was successfully changed
		uint8_t path[BUFF_SIZE];		//will store the current directory path
		if((res = f_getcwd(path, BUFF_SIZE)) != FR_OK){			//find the current directory
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR finding the current directory: %d\n",RED,res,WHITE);		//print error if the current directory could not be found
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		else{
			safe_printf("> Current directory: %s\n", path);		//else, print the current directory to the console
		}
	}
	else{		//if the directory could not be changed for another reason, print error
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: Directory could not be changed\n%s",RED,WHITE);
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return res;
	}
	return res;			//return error code (0 if successful)
}
//list contents of the current folder
FRESULT ls_function (unsigned int ArgNum, uint8_t **Arguments[])
{
	FRESULT res;		//will contain the error code for the fatfs library functions used
	DIR dir;			//create a directory object
	UINT i;
	static FILINFO fno;		//will contain the file/folder info
	static int file_count = 0;		//count of the number of files found
	static int folder_count = 0;	//count of the number of folders found
	uint8_t path[BUFF_SIZE];		//string that will contain the current path the function is looking at
	if(ArgNum != 1){			//if ls doesn't have one argument, print error. Format should be <ls>
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR 'ls' accepts no arguments\n%s",RED,WHITE);
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return 6;
	}
	if((res = f_getcwd(path, BUFF_SIZE)) != FR_OK){			//get the path of the current directory, print error if unsuccessful
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR finding the current directory: %d\n%s",RED,res,WHITE);
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
	}
	else{		//if the current directory path name was found
		safe_printf("> Current directory: %s\n", path);			//print out the current directory
		res = f_opendir(&dir, path);                       //Open the directory
		if (res == FR_OK) {		//if the directory was successfully opened
			for (;;) {		//endless loop
				res = f_readdir(&dir, &fno);                   //Read the directory item
				if (res != FR_OK || fno.fname[0] == 0) break;  //Break the for loop if an error occurs or the end of directory has been reached
				if (fno.fattrib & AM_DIR) {                    //If it is a directory
					safe_printf(">> %s\t(Folder)\n", fno.fname);		//print out folder name
					folder_count++;										//increment folder count
					sprintf(&path[i], "/%s", fno.fname);				//save the folder name to the current path
				} else {                                       //if it is a file
					safe_printf(">> %s\t(File: %d bytes)\n", fno.fname, fno.fsize);		//print the file name and file size
					int day,month,year,hour,min,sec;									//print the time and date information of the file
					day = fno.fdate & 0x1F;
					month = (fno.fdate >> 5) & 0x1F;
					year = (fno.fdate >> 9) & 0x7F;
					year = 1980+year;
					safe_printf(">>>   Date: %d/%d/%d\n", day,month,year);
					sec = fno.ftime & 0x1F;
					min = (fno.ftime >> 5) & 0x3F;
					hour = (fno.ftime >> 11) & 0x1F;
					safe_printf(">>>   Time: %d:%d:%d\n", hour,min,sec);
					file_count++;			//increment file count
				}
			}
			f_closedir(&dir);		//close the directory once it has been read
		}
		safe_printf(">> Files: %d\n", file_count);			//print the number of files encountered
		safe_printf(">> Folders: %d\n", folder_count);		//print the number of folders encountered
		file_count = 0;			//reset file count
		folder_count = 0;		//reset folder count
	}
	return res;			//return error code (0 if successful)
}
//deletes folders/files
FRESULT rm_function (unsigned int ArgNum, uint8_t **Arguments[]){
	int file;			//indicates if a file or folder was found
	FRESULT res;		//will contain the error code of the fatfs functions
	FILINFO fno;		//will contian the file/folder information
	if(ArgNum != 2){			//rm command is only allowed to have one argument; format should be <rm path>
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR 'rm' accepts one argument only\n%s",RED,WHITE);
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return 6;
	}
	//checking for file/folder existence
	if ((res = f_stat((*Arguments)[1], &fno)) != FR_OK){
		if (res == FR_NO_FILE){		//if the file/folder does not exist, print error
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: The source file/folder does not exists%s\n",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		else{		//else if another error, print error
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR checking existence of source: %d%s\n",RED, res,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		return res;
	}
	if (fno.fattrib & AM_ARC){	//if a file was found
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sSource is a file%s\n",YELLOW,WHITE);		//source is a file
		}
		file = 1;		//set the file variable
	}
	else if (fno.fattrib & AM_DIR){		//if a folder was found
		safe_printf("%sERROR: Source is a folder. %s\n",RED,WHITE);		//source is a folder
		file = 0;		//clear the file variable
	}
	else {		//else if it is not a file or folder, print error and exit function
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: Source is not a file or folder%s\n",RED,WHITE);		//source is not a file or folder
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return res;			//exit function
	}
	if (file == 1){		//if a file was found, delete the file
		if ((res = f_unlink((*Arguments)[1])) == FR_NO_PATH){			//if the path could not be found, print error
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: Could not find the path\n%s",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return res;
		}
		else if (res == FR_INVALID_NAME){		//if the path name format is invalid, print error
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: The path name format is invalid\n%s",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return res;
		}
		else if (res == FR_INVALID_OBJECT){		//if the file/directory object is invalid, print error
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: The file/directory object is invalid\n%s",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return res;
		}
		else if (res == FR_OK){		//if the file was successfully deleted, print out the deleted file name
			safe_printf("%s has been deleted\n",(*Arguments)[1]);
		}
		else{		//else if another error occured, print error
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: File could not be deleted\n%s",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return res;
		}
		return res;
	}
	else if (file == 0){		//if a folder was found, delete the folder contents before deleting the folder
		uint8_t path[BUFF_SIZE];		//contains the current directory path
		if((res = f_getcwd(path, BUFF_SIZE)) != FR_OK){		//get the current directory path, if an error occurs, print to console
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR finding the current directory: %d%s\n",RED,res,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return res;		//exit function
		}
		if(strcasecmp(path,(*Arguments)[1])==0){		//if the current directory path was found, check if it is the same as the user path input
			if (get_debug())		// if debug is turned on
			{		//if the current directory is the same as the user input, won't delete
				safe_printf("%sERROR: Current directory is the same as the directory to be deleted, won't delete\n%s", RED,WHITE);		
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return res;		//exit function
		}
		char *folder = (*Arguments)[1];		
		res = delete_directory(folder);		//call the recursive function to delete the directory contents, and delete the directory itself
		return res;
	}
}
//recursive function to delete directory
FRESULT delete_directory(uint8_t *path){
	FRESULT res;		//contain the error code returned by fatfs functions
	DIR dir;		//directory object
	UINT i;
	static FILINFO fno;		//contains the file information
	res = f_opendir(&dir,path);		//open the directory, according to the path
	if (res == FR_OK) {		//if successful
		for (;;) {		//infinite loop
			res = f_readdir(&dir, &fno);                   //Read a directory item
			if (res != FR_OK || fno.fname[0] == 0){		//if an error occured, or the end of directory was reached
				if ((res = f_unlink(path)) != FR_OK){		//delete directory, print error if unsuccessful
					if (get_debug())		// if debug is turned on
					{
						safe_printf("%sERROR: Could not find the path\n%s",RED,WHITE);
					}
					else{
						safe_printf("%sERROR%s\n",RED,WHITE);
					}
				}
				else{		//if successful, print path that has been deleted
					if (get_debug())		// if debug is turned on
					{
						safe_printf("%s%s has been deleted\n%s",YELLOW,path,WHITE);
					}
					else{
						safe_printf("%sERROR%s\n",RED,WHITE);
					}
				}
				break;  //Break on error or end of dir
			}
			if (fno.fattrib & AM_DIR) {                    // if it is a directory
				i = strlen(path);
				sprintf(&path[i], "/%s", fno.fname);		//add the directory name to the current path
				res = delete_directory(path);                    // Enter the new directory through a recursive call
				if (res != FR_OK) break;		//if an error occurs, break loop
				path[i] = 0;
			}
			else {                                       // if it is a file
				char buffer[200];
				strcpy(buffer,path);
				i = strlen(buffer);
				sprintf(&buffer[i], "/%s", fno.fname);		//add the file name to the current path
				if ((res = f_unlink(buffer)) == FR_NO_PATH){		//and delete the file
					if (get_debug())		// if debug is turned on, print error if path could not be found
					{
						safe_printf("%sERROR: Could not find the path\n%s",RED,WHITE);
					}
					else{
						safe_printf("%sERROR%s\n",RED,WHITE);
					}
				}
				else if (res == FR_INVALID_NAME){		//if path name format is invalid, print error
					if (get_debug())		// if debug is turned on
					{
						safe_printf("%sERROR: The path name format is invalid\n%s",RED,WHITE);
					}
					else{
						safe_printf("%sERROR%s\n",RED,WHITE);
					}
				}
				else if (res == FR_INVALID_OBJECT){		//if the file/directory object is invalid, print error
					if (get_debug())		// if debug is turned on
					{
						safe_printf("%sERROR: The file/directory object is invalid\n%s",RED,WHITE);
					}
					else{
						safe_printf("%sERROR%s\n",RED,WHITE);
					}
				}
				else if (res == FR_OK){		//if the file has been successfully deleted, print path that has been deleted
					if (get_debug())		// if debug is turned on
					{
						safe_printf("%s%s has been deleted\n%s",YELLOW,buffer,WHITE);
					}
					else{
						safe_printf("%sERROR%s\n",RED,WHITE);
					}
				}
				else{		//if the file could not be deleted, print error
					if (get_debug())		// if debug is turned on
					{
						safe_printf("%sERROR: File could not be delted\n%s",RED,WHITE);
					}
					else{
						safe_printf("%sERROR%s\n",RED,WHITE);
					}
				}
			}
		}
		f_closedir(&dir);		//close the directory once complete
	}
	return res;
}
//copy a file to another location
FRESULT cp_function (unsigned int ArgNum, uint8_t **Arguments[]){
	FIL fsrc, fdst;			//file object for source and destination
	BYTE buffer[4096];		//contains the data chunk to be copied one at a time from source file to destination
	FRESULT res;			//contain error code of fatfs functions
	UINT br, bw;			//contain bytes read and bytes written
	FILINFO fno;			//contain file information
	int source_file = 0; //1 if file
	int dest_file = 0;	//1 if file
	int folder_check = 0;			//1 if the folder format has been checked already
	char new_exp[BUFF_SIZE] = {};	//contain the beginning of the folder/file path
	strcpy(new_exp, "/");
	if(ArgNum != 3){				//function only accepts two extra arguments for the source and destination
		if (get_debug())		// if debug is turned on, prints error if argument number is not 3
		{
			safe_printf("%sERROR: mov requires two arguments%s\n", RED, WHITE);
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return 6;
	}
	else if(strcmp((*Arguments)[1], (*Arguments)[2]) == 0){		//if the source and destination strings at the same, print error and exit
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: source and destination are the same\n", RED,WHITE);	//print error and exit
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return 6;
	}
	//Checking for source file/folder existence
	if ((res = f_stat((*Arguments)[1], &fno)) != FR_OK){			//check for the existence of the file/folder
		if (res == FR_NO_FILE){						//if no file/folder is found, print error
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: The source file/folder does not exists%s\n",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		else{
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR checking existence of source: %d%s\n",RED, res,WHITE);
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
			safe_printf("%sSource is a file%s\n",YELLOW,WHITE);		//source is a file
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		source_file = 1;		//set source_file variable to indicate it is a file
	}
	else if (fno.fattrib & AM_DIR){		//if the source is a folder, print error and exit function (can't copy folder)
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: Source is a folder. Exiting function%s\n",RED,WHITE);		//source is a folder
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return res;			//exit function
	}
	else {		//else if there is an error, print error and exit
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: Source is not a file or folder%s\n",RED,WHITE);		//source is not a file or folder
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return res;			//exit function
	}
	//Checking for destination file/folder existence
	if ((res = f_stat((*Arguments)[2], &fno)) != FR_OK){		//if f_stat indicates no folder/file was found
		if (res == FR_NO_FILE){							//check if the path is in the form of /folder/folder/file.ext & check for existence of folder
		//this is in the case where the folder exists, but the file is going to be a new file within that folder (and thus does not exist yet)
			char ** word_array;		// output array of pointers to the start of each word
			word_array = (char * ) malloc (BUFF_SIZE * sizeof(char));		//allocate memory
			if(!word_array){			//check that memory has been successfully allocated
				if (get_debug())		// if debug is turned on
				{
					safe_printf("%sERROR: Cannot allocate memory\n%s",RED,WHITE);
				}
				else{
					safe_printf("%sERROR%s\n",RED,WHITE);
				}
				return 6;
			}
			int count;					//word count
			int j = 0;
			char character_to_search = '/';		// character that separates two words, in this case a forward slash
			count = string_parser((*Arguments)[2], &word_array,character_to_search);		//parse the string
			if (count == 1){		//if only one word was found
				if (get_debug())		// if debug is turned on
				{
					safe_printf("%sERROR: Invalid destination file/folder %s\n",RED,WHITE);		//if only one word was found, the destination is invalid
				}
				else{
					safe_printf("%sERROR%s\n",RED,WHITE);
				}
				return res;
			}
			char *end_file = (word_array)[count-1];		//let end_file point to the last word
			int exp_len = strlen(end_file);			//file the length of the last word
			char expression[BUFF_SIZE] = {};
			if (end_file[exp_len-4] == '.'){		//check if the fourth last character is a '.', signifies a file extension
				dest_file = 1;		//indicate the destination is a file
				char forward_slash[2] = "/";
				for(int i = 0; i < count-1; i++){
					strcat(expression,(word_array)[i]);		//recreate the path without the file name at the end
					if ((i+1) != count-1){
						strcat(expression,forward_slash);		//add forward slashes to the path
					}
				}
				strcat(new_exp,expression);
				if ((res = f_stat(new_exp, &fno)) != FR_OK){		//check again for the folder existence, ignoring the file name at the end of the path
					if (res == FR_NO_FILE){		//if no file/folder found, print error
						if (get_debug())		// if debug is turned on
						{
							safe_printf("%sERROR: The destination file/folder does not exists%s\n",RED,WHITE);
						}
						else{
							safe_printf("%sERROR%s\n",RED,WHITE);
						}
						return res;
					}
					else{		//else if another error, print error
						if (get_debug())		// if debug is turned on
						{
							safe_printf("%sERROR checking existence of destination: %d%s\n",RED, res,WHITE);
						}
						else{
							safe_printf("%sERROR%s\n",RED,WHITE);
						}
						return res;
					}
				}
				else{
					folder_check = 1;		//indicate that the folder existence has been checked, and exists
				}
			}
			else{		//if the last word was not in file format, print error and exit
				if (get_debug())		// if debug is turned on
				{
					safe_printf("%sERROR checking existence of destination: %d%s\n",RED, res,WHITE);
				}
				else{
					safe_printf("%sERROR%s\n",RED,WHITE);
				}
				return res;
			}
		}
		else{		//else if another error in checking destination file/folder existence, print error and exit
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR checking existence of destination: %d%s\n",RED, res,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return res;
		}
	}
	if (fno.fattrib & AM_ARC){		//if the destination is a file, print info
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sDestination is a file%s\n",YELLOW,WHITE);
		}
		dest_file = 1;		//set the dest_file variable to indicate the destination is a file
	}
	else if (fno.fattrib & AM_DIR){		//if the destination is a folder
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sDestination is a folder%s\n",YELLOW,WHITE);
		}
		char forward_slash[2] = "/";
		if (folder_check == 0){		//if the destination path leads to a folder but does not have a file name at the end of the path
			//adds the source file name to the end of the path, if a file name has not been specified
			if(source_file == 1){			//if source is a path to a file (ie has a file name), not just a folder name
				char ** words;		// output array of pointers to the start of each word
				words = (char * ) malloc (BUFF_SIZE * sizeof(char));		//allocate memory
				if(!words){			//check that memory has been successfully allocated
					if (get_debug())		// if debug is turned on
					{
						safe_printf("%sERROR: Cannot allocate memory\n%s",RED,WHITE);
					}
					else{
						safe_printf("%sERROR%s\n",RED,WHITE);
					}
					return 6;
				}
				int count;					//word count
				char character_to_search = '/';		// character that separates two words, in this case a forward slash
				count = string_parser((*Arguments)[1], &words,character_to_search);		//parse the string
				if (count != 1){
					//let the file name to be added to the destination file path, equal the last word
					strcat((*Arguments)[2],forward_slash);		//add a forward slash to the destination path before adding the file name
					strcat((*Arguments)[2],(words)[count-1]);		//add the file name
					if (get_debug())		// if debug is turned on
					{
						safe_printf("%sDestination folder: %s %s\n",PURPLE,(*Arguments)[2],WHITE);
					}
				}
				else if (count == 1){		//if the source path is just a file name
					strcat((*Arguments)[2],forward_slash);			//add a forward slash
					strcat((*Arguments)[2],(*Arguments)[1]);		//add the file name to the end of the destination path
				}
			}
		}
	}
	else {		//if the destination is not a file or folder, print error and exit function
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: Destination is not a file or folder%s\n",RED,WHITE);
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return res;
	}
	//check if the destination file is the same as the current
	if (dest_file == 1){		//if the destination is a file
		uint8_t path[BUFF_SIZE];
		if((res = f_getcwd(path, BUFF_SIZE)) != FR_OK){		//find the current directory path
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR finding the current directory: %d%s\n",RED,res,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return res;
		}
		char ** word_array;		// output array of pointers to the start of each word
		word_array = (char * ) malloc (BUFF_SIZE * sizeof(char));		//allocate memory
		if(!word_array){			//check that memory has been successfully allocated
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: Cannot allocate memory\n%s",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return 6;
		}
		int count;		//word count
		char character_to_search = '/';		// character that separates two words, in this case a forward slash
		count = string_parser((*Arguments)[2], &word_array,character_to_search);		//parse the string
		char *end_file = (word_array)[count-1];			//let end_file point to the last word (ie the file name)
		int exp_len = strlen(end_file);			//find the length of the file name
		char expression[BUFF_SIZE] = {};
		if (end_file[exp_len-4] == '.'){		//check if the fourth last character is a '.', signifies a file extension
			char forward_slash[2] = "/";
			for(int i = 0; i < count-1; i++){
				strcat(expression,(word_array)[i]);		//add words to the path without file name
				if ((i+1) != count-1){
					strcat(expression,forward_slash);	//add forward slashes to the path
				}
			}
			strcat(new_exp,expression);
		}
		if(strcasecmp(path,new_exp)==0){		//check if the current directory is the same as the destination
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: Current directory is the same as the destination\n%s",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return res;
		}
	}
	//Open source file and print errors if required
	if ((res = f_open(&fsrc, (*Arguments)[1], FA_READ)) != FR_OK){
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sError opening source file: %d%s\n",RED,res,WHITE);
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		if(res == FR_NO_PATH){
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: Could not find the path%s\n",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		else if (res == FR_INVALID_NAME){
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: The path name format is invalid%s\n",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		else if (res == FR_INVALID_OBJECT){
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: The file/directory object is invalid%s\n",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		return res;
	}
	//Create destination file and print errors if required
	if ((res = f_open(&fdst, (*Arguments)[2], FA_WRITE | FA_CREATE_ALWAYS)) != FR_OK){
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%s\tError creating destination file: %d%s\n",RED,res,WHITE);
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		if(res == FR_NO_PATH){
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: Could not find the path%s\n",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		else if (res == FR_INVALID_NAME){
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: The path name format is invalid%s\n",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		else if (res == FR_INVALID_OBJECT){
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: The file/directory object is invalid%s\n",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		return res;
	}
	//Copy source to destination
	for (;;) {
		res = f_read(&fsrc, buffer, sizeof buffer, &br);  //Read a chunk of source file
		if (res || br == 0) break; //error or eof
		res = f_write(&fdst, buffer, br, &bw);            //Write it to the destination file
		if (res || bw < br) break; //error or disk full
	}
	//Close open files
	f_close(&fsrc);
	f_close(&fdst);
	return res;
}

FRESULT mov_function (unsigned int ArgNum, uint8_t **Arguments[]){
	BYTE buffer[4096];		//contains the data chunk to be copied one at a time from source file to destination
	FRESULT res;			//contain error code of fatfs functions
	UINT br, bw;			//contain bytes read and bytes written
	FILINFO fno;			//contain file information
	int source_file = 0; 	//1 if file
	int dest_file = 0;		//1 if file
	int folder_check = 0;			//1 if the folder format has been checked already
	char new_exp[BUFF_SIZE] = {};	//contain the beginning of the folder/file path
	strcpy(new_exp, "/");
	if(ArgNum != 3){				//function only accepts two extra arguments for the source and destination
		if (get_debug())		// if debug is turned on, prints error if argument number is not 3
		{
			safe_printf("%sERROR: mov requires two arguments%s\n", RED, WHITE);
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return 6;
	}
	else if(strcmp((*Arguments)[1], (*Arguments)[2]) == 0){		//if the source and destination strings at the same, print error and exit
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: source and destination are the same\n", RED,WHITE);	//print error and exit
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return 6;
	}
	//Checking for source file/folder existence
	if ((res = f_stat((*Arguments)[1], &fno)) != FR_OK){			//check for the existence of the file/folder
		if (res == FR_NO_FILE){						//if no file/folder is found, print error
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: The source file/folder does not exists%s\n",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		else{
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR checking existence of source: %d%s\n",RED, res,WHITE);
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
			safe_printf("%sSource is a file%s\n",YELLOW,WHITE);		//source is a file
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		source_file = 1;		//set source_file variable to indicate it is a file
	}
	else if (fno.fattrib & AM_DIR){		//if the source is a folder, print error and exit function (can't copy folder)
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: Source is a folder. Exiting function%s\n",RED,WHITE);		//source is a folder
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return res;			//exit function
	}
	else {		//else if there is an error, print error and exit
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: Source is not a file or folder%s\n",RED,WHITE);		//source is not a file or folder
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return res;			//exit function
	}
	//Checking for destination file/folder existence
	if ((res = f_stat((*Arguments)[2], &fno)) != FR_OK){		//if f_stat indicates no folder/file was found
		if (res == FR_NO_FILE){							//check if the path is in the form of /folder/folder/file.ext & check for existence of folder
		//this is in the case where the folder exists, but the file is going to be a new file within that folder (and thus does not exist yet)
			char ** word_array;		// output array of pointers to the start of each word
			word_array = (char * ) malloc (BUFF_SIZE * sizeof(char));		//allocate memory
			if(!word_array){			//check that memory has been successfully allocated
				if (get_debug())		// if debug is turned on
				{
					safe_printf("%sERROR: Cannot allocate memory\n%s",RED,WHITE);
				}
				else{
					safe_printf("%sERROR%s\n",RED,WHITE);
				}
				return 6;
			}
			int count;					//word count
			int j = 0;
			char character_to_search = '/';		// character that separates two words, in this case a forward slash
			count = string_parser((*Arguments)[2], &word_array,character_to_search);		//parse the string
			if (count == 1){		//if only one word was found
				if (get_debug())		// if debug is turned on
				{
					safe_printf("%sERROR: Invalid destination file/folder %s\n",RED,WHITE);		//if only one word was found, the destination is invalid
				}
				else{
					safe_printf("%sERROR%s\n",RED,WHITE);
				}
				return res;
			}
			char *end_file = (word_array)[count-1];		//let end_file point to the last word
			int exp_len = strlen(end_file);			//file the length of the last word
			char expression[BUFF_SIZE] = {};
			if (end_file[exp_len-4] == '.'){		//check if the fourth last character is a '.', signifies a file extension
				dest_file = 1;		//indicate the destination is a file
				char forward_slash[2] = "/";
				for(int i = 0; i < count-1; i++){
					strcat(expression,(word_array)[i]);		//recreate the path without the file name at the end
					if ((i+1) != count-1){
						strcat(expression,forward_slash);		//add forward slashes to the path
					}
				}
				strcat(new_exp,expression);
				if ((res = f_stat(new_exp, &fno)) != FR_OK){		//check again for the folder existence, ignoring the file name at the end of the path
					if (res == FR_NO_FILE){		//if no file/folder found, print error
						if (get_debug())		// if debug is turned on
						{
							safe_printf("%sERROR: The destination file/folder does not exists%s\n",RED,WHITE);
						}
						else{
							safe_printf("%sERROR%s\n",RED,WHITE);
						}
						return res;
					}
					else{		//else if another error, print error
						if (get_debug())		// if debug is turned on
						{
							safe_printf("%sERROR checking existence of destination: %d%s\n",RED, res,WHITE);
						}
						else{
							safe_printf("%sERROR%s\n",RED,WHITE);
						}
						return res;
					}
				}
				else{
					folder_check = 1;		//indicate that the folder existence has been checked, and exists
				}
			}
			else{		//if the last word was not in file format, print error and exit
				if (get_debug())		// if debug is turned on
				{
					safe_printf("%sERROR checking existence of destination: %d%s\n",RED, res,WHITE);
				}
				else{
					safe_printf("%sERROR%s\n",RED,WHITE);
				}
				return res;
			}
		}
		else{		//else if another error in checking destination file/folder existence, print error and exit
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR checking existence of destination: %d%s\n",RED, res,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return res;
		}
	}
	if (fno.fattrib & AM_ARC){		//if the destination is a file, print info
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sDestination is a file%s\n",YELLOW,WHITE);
		}
		dest_file = 1;		//set the dest_file variable to indicate the destination is a file
	}
	else if (fno.fattrib & AM_DIR){		//if the destination is a folder
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sDestination is a folder%s\n",YELLOW,WHITE);
		}
		char forward_slash[2] = "/";
		if (folder_check == 0){		//if the destination path leads to a folder but does not have a file name at the end of the path
			//adds the source file name to the end of the path, if a file name has not been specified
			if(source_file == 1){			//if source is a path to a file (ie has a file name), not just a folder name
				char ** words;		// output array of pointers to the start of each word
				words = (char * ) malloc (BUFF_SIZE * sizeof(char));		//allocate memory
				if(!words){			//check that memory has been successfully allocated
					if (get_debug())		// if debug is turned on
					{
						safe_printf("%sERROR: Cannot allocate memory\n%s",RED,WHITE);
					}
					else{
						safe_printf("%sERROR%s\n",RED,WHITE);
					}
					return 6;
				}
				int count;					//word count
				char character_to_search = '/';		// character that separates two words, in this case a forward slash
				count = string_parser((*Arguments)[1], &words,character_to_search);		//parse the string
				if (count != 1){
					//let the file name to be added to the destination file path, equal the last word
					strcat((*Arguments)[2],forward_slash);		//add a forward slash to the destination path before adding the file name
					strcat((*Arguments)[2],(words)[count-1]);		//add the file name
					if (get_debug())		// if debug is turned on
					{
						safe_printf("%sDestination folder: %s %s\n",PURPLE,(*Arguments)[2],WHITE);
					}
				}
				else if (count == 1){		//if the source path is just a file name
					strcat((*Arguments)[2],forward_slash);			//add a forward slash
					strcat((*Arguments)[2],(*Arguments)[1]);		//add the file name to the end of the destination path
				}
			}
		}
	}
	else {		//if the destination is not a file or folder, print error and exit function
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: Destination is not a file or folder%s\n",RED,WHITE);
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return res;
	}
	//check if the destination file is the same as the current
	if (dest_file == 1){		//if the destination is a file
		uint8_t path[BUFF_SIZE];
		if((res = f_getcwd(path, BUFF_SIZE)) != FR_OK){		//find the current directory path
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR finding the current directory: %d%s\n",RED,res,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return res;
		}
		char ** word_array;		// output array of pointers to the start of each word
		word_array = (char * ) malloc (BUFF_SIZE * sizeof(char));		//allocate memory
		if(!word_array){			//check that memory has been successfully allocated
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: Cannot allocate memory\n%s",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return 6;
		}
		int count;		//word count
		char character_to_search = '/';		// character that separates two words, in this case a forward slash
		count = string_parser((*Arguments)[2], &word_array,character_to_search);		//parse the string
		char *end_file = (word_array)[count-1];			//let end_file point to the last word (ie the file name)
		int exp_len = strlen(end_file);			//find the length of the file name
		char expression[BUFF_SIZE] = {};
		if (end_file[exp_len-4] == '.'){		//check if the fourth last character is a '.', signifies a file extension
			char forward_slash[2] = "/";
			for(int i = 0; i < count-1; i++){
				strcat(expression,(word_array)[i]);		//add words to the path without file name
				if ((i+1) != count-1){
					strcat(expression,forward_slash);	//add forward slashes to the path
				}
			}
			strcat(new_exp,expression);
		}
		if(strcasecmp(path,new_exp)==0){		//check if the current directory is the same as the destination
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: Current directory is the same as the destination\n%s",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return res;
		}
	}
	//Move source file to destination
	if (res = f_rename((*Arguments)[1],(*Arguments)[2]) != FR_OK){
		if(res == FR_NO_PATH){		//if the path could not be found, print error
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: Could not find the path%s\n",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		else if (res == FR_INVALID_NAME){		//if the path name format is invalid, print error
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: The path name format is invalid%s\n",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		else if (res == FR_INVALID_OBJECT){		//if the file/directory object is invalid, print error
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: The file/directory object is invalid%s\n",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		else{			//if another error occured, print error
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: Cannot move %s%s\n",RED,(*Arguments)[1],WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		return res;
	}
	else{		//else successfully moved file to destination
		safe_printf("%s%s has been moved to %s%s\n",YELLOW, (*Arguments)[1],(*Arguments)[2],WHITE);
	}
	return res;
}
//Create a new directory
FRESULT mkdir_function (unsigned int ArgNum, uint8_t **Arguments[]){
	FRESULT res;		//file object for source and destination
	if(ArgNum != 2){		//if argument number is not 2, print error and exit
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: mkdir requires one argument only\n%s",RED,WHITE);
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
		return 6;
	}
	else if((res = f_mkdir((*Arguments)[1])) != FR_OK){		//if an error occurred while making new directory
		if (res == FR_NO_PATH){		//if the path could not be found, print error
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: Could not find the path\n%s",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return res;
		}
		else if (res == FR_INVALID_NAME){		//if the path name format is invalid, print error
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: The path name format is invalid\n%s",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return res;
		}
		else if (res == FR_INVALID_OBJECT){		//if the file/directory object is invalid, print error
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: The file/directory object is invalid\n%s",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return res;
		}
		else{		//else print error if another error type occurred
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: New folder could not be created\n%s",RED,WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
			return res;
		}
	}
	else{		//else if successful, print new folder name
		safe_printf("> New folder: %s\n", (*Arguments)[1]);
		return res;
	}
}

FRESULT HelpFunction(unsigned int ArgNum,uint8_t **Arguments[])
{
	const command_s CommandList[] = {
			{"ls", &ls_function, "Lists contents of the current folder", 0},
			{"cd", &cd_function, "Change the folder to <dir>", 0},
			{"mkdir", &mkdir_function, "Create a new folder <dir>", 0},
			{"cp", &cp_function, "copy the file to another destination: cp <scr> <dst>", 0},
			{"rm", &rm_function, "remove the file or folder: rm <scr>", 0},
			{"debug", &debug_function, "change debug", 1},
			{"analog", &speed_function, "set speed", 1},
			{"help", &HelpFunction, "View command list", 0},
			{"mov", &mov_function, "Move source file to destination: mov <scr> <dst>", 0},
			{"memory", &memory_setting_function, "set memory", 1},
			{NULL, NULL, NULL}
	};
	if (ArgNum == 1)		//if the command was help, print all command helpstrings
	{
		for (int k = 0; k < FUNC_NUM; k++)
		{
			safe_printf(">> %s: %s\n", Commandlist[k].NameString, Commandlist[k].HelpString); //print the help strings of all the commands
		}
	}
	else{
		for (int k = 0; k < FUNC_NUM; k++){		//cycle through the structure for the command

			if (strcmp((*Arguments)[1], Commandlist[k].NameString)==0){		//if the command is found
				safe_printf(">> %s: %s\n", Commandlist[k].NameString, Commandlist[k].HelpString);		//print the helpstring corresponding to the command
				break;
			}
		}
	}
	return 0;
}

void debug_function(unsigned int ArgNum, uint8_t **Arguments[])
{
	if (ArgNum == 1)
	{
		// display debug
		if (get_debug() == 1)
		{
			safe_printf("%sDebug is currently on\n%s", GREEN, WHITE);
		}
		else
		{
			safe_printf("%sDebug is currently off\n%s", GREEN, WHITE);
		}
	}
	else if (strcmp(((*Arguments)[1]), "on") == 0)
	{
		// debug on entered
		safe_printf("%sDebug turned on \n%s", GREEN, WHITE);
		set_debug(1);
	}
	else if (strcmp(((*Arguments)[1]), "off") == 0)
	{
		// debug off entered
		safe_printf("%sDebug turned off \n%s", GREEN, WHITE);
		set_debug(0);
	}
	else
	{
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: debug requires correct arguments only\n%s", RED, WHITE);
		}
	}
}

void speed_function(unsigned int ArgNum, uint8_t **Arguments[])
{
	if (ArgNum != 2)
	{
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: speed requires 2 arguments only\n%s", RED, WHITE);
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
	}
	else
	{
		int speed_number = atoi((*Arguments)[1]);
		if ((speed_number < 1) || (speed_number > 50))
		{
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: number entered is out of range\n%s", RED, WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		else
		{
			safe_printf("%sSpeed set to %d \n%s", CYAN, speed_number, WHITE);
			set_speed(speed_number);
		}
	}
}

void memory_setting_function (unsigned int ArgNum, uint8_t **Arguments[])
{
	if (ArgNum != 2)
	{
		if (get_debug())		// if debug is turned on
		{
			safe_printf("%sERROR: memory requires 2 arguments only\n%s", RED, WHITE);
		}
		else{
			safe_printf("%sERROR%s\n",RED,WHITE);
		}
	}
	else
	{
		int memory_input = atoi((*Arguments)[1]);
		if ((memory_input < 1) || (memory_input > MEM_NO))
		{
			if (get_debug())		// if debug is turned on
			{
				safe_printf("%sERROR: memory number entered is out of range\n%s", RED, WHITE);
			}
			else{
				safe_printf("%sERROR%s\n",RED,WHITE);
			}
		}
		else
		{
			safe_printf("%sMemory set to %d from putty\n%s", CYAN, memory_input, WHITE);
			change_memory(memory_input);
		}
	}
}
