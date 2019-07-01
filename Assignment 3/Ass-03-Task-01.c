#include "Ass-03.h"
#define DEBUG_P

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
#define FUNC_NUM 6
#define AEST 10

void Ass_03_Task_01(void const * argument)
{
	uint32_t loop=0;
	uint8_t ts[100];
	uint16_t i;
	Coordinate display;
	char c;

	safe_printf("Hello from Task 1 - Console (serial input)\n");
	safe_printf("INFO: Initialise LCD and TP first...\n");

	// STEPIEN: Initialize and turn on LCD and calibrate the touch panel
	BSP_LCD_Init();
	BSP_LCD_DisplayOn();
	BSP_TP_Init();

	// Signal other tasks to start
	osSignalSet(myTask02Handle, 1);
	osSignalSet(myTask03Handle, 1);
	osSignalSet(myTask04Handle, 1);

	// Display welcome message
	osMutexWait(myMutex01Handle, osWaitForever);
	BSP_LCD_Clear(LCD_COLOR_WHITE);
	BSP_LCD_SetFont(&Font16);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DrawRect(5, 5, 58, 52);
	BSP_LCD_DrawRect(68, 5, 58, 52);
	BSP_LCD_DrawRect(131, 5, 58, 52);
	BSP_LCD_DrawRect(194, 5, 58, 52);
	BSP_LCD_DrawRect(257, 5, 58, 52);
	BSP_LCD_DrawRect(5, 62, 58, 52);
	BSP_LCD_DrawRect(5, 119, 58, 52);
	BSP_LCD_DrawRect(5, 176, 58, 52);
	BSP_LCD_DisplayStringAt(32, 22, (uint8_t*) "Load", CENTER_MODE);
	BSP_LCD_DisplayStringAt(286, 22, (uint8_t*) "Debug", CENTER_MODE);
	BSP_LCD_DisplayStringAt(32, 79, (uint8_t*) "Mem1", CENTER_MODE);
	BSP_LCD_DisplayStringAt(32, 136, (uint8_t*) "Mem2", CENTER_MODE);
	BSP_LCD_DisplayStringAt(32, 193, (uint8_t*) "Mem3", CENTER_MODE);
	drawplay(149, 20);
	drawstop(212, 20);
	drawsave(97, 31);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&Font8);
	BSP_LCD_DisplayStringAt(160, 230, (uint8_t*) "JOSHUA BEVERLEY & RADHIKA FERON ELEC3730 ASSIGNMENT 3", CENTER_MODE);
	BSP_LCD_SetFont(&Font16);
	osMutexRelease(myMutex01Handle);

	debug_init();

	const command_s CommandList[] = {
			{"ls", &ls_function, "Lists contents of the current folder", 0},
			{"cd", &cd_function, "Change the folder to <dir>", 0},
			{"mkdir", &mkdir_function, "Create a new folder <dir>", 0},
			{"cp", &cp_function, "copy the file to another destination: cp <scr> <dst>", 0},
			{"rm", &rm_function, "remove the file or folder: rm <scr>", 0},
			{"debug", &debug_function, "change debug", 1},
			{"analog", &speed_function, "set speed", 1},
			{NULL, NULL, NULL}
	};
	static char *expression;			//will contain the string that is typed in putty
	expression = (char *) malloc(BUFF_SIZE * sizeof(char));	// malloc
	if(!expression)
		return;
	int position = 0;				//position in the string to add the character read

	while (1)
	{
		//		char buffer[20] = {0};
		//		time_t now;
		//		struct tm* mytime = localtime(&now);
		//		safe_printf("sec = %d\n", mytime->tm_sec);
		//		safe_printf("min = %d\n", mytime->tm_min);
		//		safe_printf("hour = %d\n", mytime->tm_hour);
		//		safe_printf("day = %d\n", mytime->tm_yday);
		//		safe_printf("month = %d\n", mytime->tm_mon);

		//		time_t rawtime;
		//		struct tm *info;
		//		time(&rawtime);
		//		/* Get GMT time */
		//		info = gmtime(&rawtime);
		//
		//		safe_printf("Current world clock:\n");
		//		safe_printf("Australia time: %2d:%02d:%02d\n", (info->tm_hour+AEST)%24, info->tm_min, info->tm_sec);
		//		   safe_printf("Australia date: %2d:%02d:%02d\n", info->tm_year, info->tm_mon, info->tm_mday);

			c = getchar();
			safe_printf("%c", c);
			fflush(stdout);
			//HAL_GPIO_TogglePin(GPIOD, LD4_Pin);  // Toggle LED4

			expression[position] = c;
			position++;

			// If we get a return character then process the string
			if (c == '\r' || position > 101) {
				safe_printf("\n");
				expression[position - 1] = '\0';
				char ** array_of_words;		// output array of pointers to the start of each world
				array_of_words = (char * ) malloc (BUFF_SIZE * sizeof(int));
				if(!array_of_words)
					return;
				int count;					//word count
				int j = 0;
				char character_to_search = ' ';		// character that separates two words, in this case a space
				count = string_parser(expression, &array_of_words,character_to_search);
				position = 0;
				expression[position] = '\0';
				safe_printf("Word count = %d\n", count);

				//				for (int j = 0; j<count; j++){
				//					safe_printf("Word %d: %s\n", j+1, (array_of_words)[j]);
				//				}
				int position = -1;
				for (int k=0; k < FUNC_NUM; k++){
					if(strcmp(array_of_words[0],CommandList[k].NameString)==0){
						position = k;
						break;
					}
				}
				if(position == -1){
					safe_printf("%sError: Command not found\n%s",RED,WHITE);
				}
				else if (CommandList[position].Type == 0){
					FRESULT res;
					//					if (res = f_rename("Hello.txt","/TESTDIR/Hello.txt") != FR_OK){
					//						safe_printf("\tError code: %d\n",res);
					//					}
					osMutexWait(myMutex01Handle, osWaitForever);
					res = CommandList[position].Function_p(count,&array_of_words);
					osMutexRelease(myMutex01Handle);
				}
				else if (CommandList[position].Type == 1)
				{
					osMutexWait(myMutex01Handle, osWaitForever);
					CommandList[position].Function_p(count,&array_of_words);
					osMutexRelease(myMutex01Handle);
				}
//				int values[19] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
//				myWriteFile(&values, "Test.txt",19);
//				int bytesread;
//				int *read_val = myReadFile(&bytesread, "Test.txt");
//				safe_printf("OUTSIDE FUNCTION: %d\n", bytesread);
//				for (int i = 0; i<bytesread/sizeof(int);i++){
//					safe_printf("%f\n", read_val[i]);
//				}
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
	if(!copy)
			        return -1;
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
FRESULT cd_function(unsigned int ArgNum, uint8_t **Arguments[]){
	FRESULT res;
	//******************check for invalid arguments
	if((res = f_chdir(((*Arguments)[1]))) == FR_NO_PATH){
		safe_printf("%sERROR: Could not find the path\n%s",RED,WHITE);
		return res;
	}
	else if (res == FR_INVALID_NAME){
		safe_printf("%sERROR: The path name format is invalid%s\n",RED,WHITE);
		return res;
	}
	else if (res == FR_INVALID_OBJECT){
		safe_printf("%sERROR: The file/directory object is invalid%s\n",RED,WHITE);
		return res;
	}
	else if (res == FR_OK){
		uint8_t path[BUFF_SIZE];
		if((res = f_getcwd(path, BUFF_SIZE)) != FR_OK){
			safe_printf("%sERROR finding the current directory: %d\n",RED,res,WHITE);
		}
		else{
			safe_printf("> Current directory: %s\n", path);
		}
	}
	else{
		safe_printf("%sERROR: Directory could not be changed\n%s",RED,WHITE);
		return res;
	}
	return res;
}
//list contents of the current folder
FRESULT ls_function (unsigned int ArgNum, uint8_t **Arguments[])/* Start node to be scanned (***also used as work area***) */
{
	FRESULT res;
	DIR dir;
	UINT i;
	static FILINFO fno;
	static int file_count = 0;
	static int folder_count = 0;
	uint8_t path[BUFF_SIZE];
	if(ArgNum != 1){
		safe_printf("%sERROR 'ls' accepts no arguments\n%s",RED,WHITE);
	}
	if((res = f_getcwd(path, BUFF_SIZE)) != FR_OK){
		safe_printf("%sERROR finding the current directory: %d\n%s",RED,res,WHITE);
	}
	else{
		safe_printf("> Current directory: %s\n", path);
		res = f_opendir(&dir, path);                       /* Open the directory */
		if (res == FR_OK) {
			for (;;) {
				res = f_readdir(&dir, &fno);                   /* Read a directory item */
				if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
				if (fno.fattrib & AM_DIR) {                    /* It is a directory */
					//i = strlen(path);
					safe_printf(">> %s\t(Folder)\n", fno.fname);
					folder_count++;
					sprintf(&path[i], "/%s", fno.fname);
					//res = scan_files(path);                    /* Enter the directory */
					//if (res != FR_OK) break;
					//path[i] = 0;
				} else {                                       /* It is a file. */
					safe_printf(">> %s\t(File: %d bytes)\n", fno.fname, fno.fsize);
					//					int day,month,year,hour,min,sec;
					//					day = fno.fdate & 0x1F;
					//					month = (fno.fdate >> 5) & 0x1F;
					//					year = (fno.fdate >> 9) & 0x7F;
					//					year = 1980+year;
					//					safe_printf(">>>   Date: %d/%d/%d\n", day,month,year);
					//					sec = fno.ftime & 0x1F;
					//					min = (fno.ftime >> 5) & 0x3F;
					//					hour = (fno.ftime >> 11) & 0x1F;
					//					safe_printf(">>>   Time: %d:%d:%d\n", hour,min,sec);

					//safe_printf("%s/%s\n", path, fno.fname);2
					file_count++;
				}
			}
			f_closedir(&dir);
		}
		safe_printf(">> Files: %d\n", file_count);
		safe_printf(">> Folders: %d\n", folder_count);
		file_count = 0;
		folder_count = 0;
	}
	return res;
}
FRESULT rm_function (unsigned int ArgNum, uint8_t **Arguments[]){
	int file;
	FRESULT res;
	FILINFO fno;
	//checking for file/folder existence
	if ((res = f_stat((*Arguments)[1], &fno)) != FR_OK){
		if (res == FR_NO_FILE){
			safe_printf("%sERROR: The source file/folder does not exists%s\n",RED,WHITE);
		}
		else{
			safe_printf("%sERROR checking existence of source: %d%s\n",RED, res,WHITE);
		}
		return res;
	}
	if (fno.fattrib & AM_ARC){
		safe_printf("%sSource is a file%s\n",YELLOW,WHITE);		//source is a file
		file = 1;
	}
	else if (fno.fattrib & AM_DIR){
		safe_printf("%sERROR: Source is a folder. %s\n",RED,WHITE);		//source is a folder
		file = 0;
		//return res;			//exit function
	}
	else {
		safe_printf("%sERROR: Source is not a file or folder%s\n",RED,WHITE);		//source is not a file or folder
		return res;			//exit function
	}
	//after checking file or folder, if you reach this point, it must be valid....

	//check if file or folder (same method???)
	//if file exists, delete file
	//if folder...cannot delete sub-directory is not empty...(maybe empty entire sub-directory?
	if (file == 1){
		if ((res = f_unlink((*Arguments)[1])) == FR_NO_PATH){
			//********************
			safe_printf("%sERROR: Could not find the path\n%s",RED,WHITE);
			return res;
		}
		else if (res == FR_INVALID_NAME){
			safe_printf("%sERROR: The path name format is invalid\n%s",RED,WHITE);
			return res;
		}
		else if (res == FR_INVALID_OBJECT){
			safe_printf("%sERROR: The file/directory object is invalid\n%s",RED,WHITE);
			return res;
		}
		else if (res == FR_OK){
			safe_printf("%s has been deleted\n",(*Arguments)[1]);
		}
		else{
			safe_printf("%sERROR: Directory could not be changed\n%s",RED,WHITE);
			return res;
		}
		return res;
	}
	else if (file == 0){
		uint8_t path[BUFF_SIZE];
		if((res = f_getcwd(path, BUFF_SIZE)) != FR_OK){
			safe_printf("%sERROR finding the current directory: %d%s\n",RED,res,WHITE);
			return res;
		}
		if(strcasecmp(path,(*Arguments)[1])==0){
			safe_printf("%sERROR: Current directory is the same as the destination, won't delete\n%s", RED,WHITE);
			return res;
		}
		char *folder = (*Arguments)[1];
		//safe_printf("folder: %s\n", folder);
		res = delete_directory(folder);
		return res;
	}
}

FRESULT delete_directory(uint8_t *path){
	FRESULT res;
	DIR dir;
	UINT i;
	static FILINFO fno;
	res = f_opendir(&dir,path);
	if (res == FR_OK) {
		for (;;) {
			res = f_readdir(&dir, &fno);                   /* Read a directory item */
			if (res != FR_OK || fno.fname[0] == 0){
				if ((res = f_unlink(path)) != FR_OK){
					safe_printf("%sERROR: Could not find the path\n%s",RED,WHITE);
				}
				else{
					safe_printf("%s%s has been deleted\n%s",YELLOW,path,WHITE);
				}
				break;  /* Break on error or end of dir */
			}
			if (fno.fattrib & AM_DIR) {                    /* It is a directory */
				i = strlen(path);
				sprintf(&path[i], "/%s", fno.fname);
				res = delete_directory(path);                    /* Enter the directory */
				if (res != FR_OK) break;
				path[i] = 0;
			}
			else {                                       /* It is a file. */
				char buffer[200];
				strcpy(buffer,path);
				i = strlen(buffer);
				sprintf(&buffer[i], "/%s", fno.fname);
				if ((res = f_unlink(buffer)) == FR_NO_PATH){
					//********************
					safe_printf("%sERROR: Could not find the path\n%s",RED,WHITE);
				}
				else if (res == FR_INVALID_NAME){
					safe_printf("%sERROR: The path name format is invalid\n%s",RED,WHITE);
				}
				else if (res == FR_INVALID_OBJECT){
					safe_printf("%sERROR: The file/directory object is invalid\n%s",RED,WHITE);
				}
				else if (res == FR_OK){
					safe_printf("%s%s has been deleted\n%s",YELLOW,buffer,WHITE);
				}
				else{
					safe_printf("%sERROR: Directory could not be changed:%d\n%s",RED,res,WHITE);
				}
			}
		}
		f_closedir(&dir);
	}

	return res;
}

FRESULT cp_function (unsigned int ArgNum, uint8_t **Arguments[]){
	//cp <scr> <dist>
	//copy file <scr> to destination <dst>
	//if <dst> = folder, file copied to that folder
	//if <dst> != folder, copy of file made to current folder
	//what if destination file already exists?
	//	cp file1.csv file2.csv -> new file: file2.csv
	//	cp file1.csv Heart -> new file: heart/heart.csv

	// Look if it is a file or a folder (from the input)
	// if the destination is a file, copy the file to the current folder

	//check if the file already exists and either overwrite it or add (1) to it

	//maybe check for existence of the file/folder

	//********************************************http://elm-chan.org/fsw/ff/doc/open.html to copy, not just move it.
	FIL fsrc, fdst;
	BYTE buffer[4096];
	FRESULT res;
	UINT br, bw;
	FILINFO fno;
	int source_file = 0; //1 if file
	int dest_file = 0;	//1 if file
	int folder_check = 0;
	char new_exp[BUFF_SIZE] = {};
	strcpy(new_exp, "/");
	if(ArgNum != 3){				//function only accepts two extra arguments for the source and destination
		safe_printf("%sERROR: cp requires two arguments%s\n", RED, WHITE);
		return 6;
	}
	else if(strcmp((*Arguments)[1], (*Arguments)[2]) == 0){							//if the source and destination strings at the same
		safe_printf("%sERROR: source and destination are the same\n", RED,WHITE);	//print error and exit
		return 6;
	}
	//checking for source file/folder existence
	if ((res = f_stat((*Arguments)[1], &fno)) != FR_OK){			//check for the existence of the file/folder
		if (res == FR_NO_FILE){						//if no file/folder is found
			safe_printf("%sERROR: The source file/folder does not exists%s\n",RED,WHITE);
		}
		else{
			safe_printf("%sERROR checking existence of source: %d%s\n",RED, res,WHITE);
		}
		return res;
	}
	if (fno.fattrib & AM_ARC){
		safe_printf("%sSource is a file%s\n",YELLOW,WHITE);		//source is a file
		source_file = 1;
	}
	else if (fno.fattrib & AM_DIR){
		safe_printf("%sERROR: Source is a folder. Exiting function%s\n",RED,WHITE);		//source is a folder
		return res;			//exit function
	}
	else {
		safe_printf("%sERROR: Source is not a file or folder%s\n",RED,WHITE);		//source is not a file or folder
		return res;			//exit function
	}
	//checking for destination file/folder existence
	if ((res = f_stat((*Arguments)[2], &fno)) != FR_OK){
		if (res == FR_NO_FILE){				//check if in the form of /folder/folder/file.ext & check for existence of folder
			char ** word_array;		// output array of pointers to the start of each word
			word_array = (char * ) malloc (BUFF_SIZE * sizeof(char));
			if(!word_array)
						        return 6;
			int count;					//word count
			int j = 0;
			char character_to_search = '/';		// character that separates two words, in this case a forward slash
			count = string_parser((*Arguments)[2], &word_array,character_to_search);		//parse the string
			if (count == 1){
				safe_printf("%sERROR: Invalid destination file/folder %s\n",RED,WHITE);		//if only one word was found, the destination is invalid
				return res;
			}
			//			safe_printf("%Word_array: %s%s\n",YELLOW, (word_array)[count-1],WHITE);
			char *end_file = (word_array)[count-1];
			//			safe_printf("%sExpression: %s%s\n",YELLOW, end_file,WHITE);
			//			safe_printf("%Word_array: %s%s\n",YELLOW, (word_array)[0],WHITE);
			int exp_len = strlen(end_file);
			char expression[BUFF_SIZE] = {};
			if (end_file[exp_len-4] == '.'){		//check if the fourth last character is a '.', signifies a file extension
				//				safe_printf("\tExpression has an extension: Destination is a file?\n");
				dest_file = 1;
				char forward_slash[2] = "/";
				for(int i = 0; i < count-1; i++){
					strcat(expression,(word_array)[i]);
					//					safe_printf("%New expression: %s %s\n",PURPLE,expression,WHITE);			//add words to the path
					if ((i+1) != count-1){
						strcat(expression,forward_slash);										//add forward slashes to the path
						//						safe_printf("%c expression: %s %s\n",PURPLE,expression,WHITE);
					}
				}
				strcat(new_exp,expression);
				//				safe_printf("%sFinal expression: %s %s\n",PURPLE,new_exp,WHITE);
				if ((res = f_stat(new_exp, &fno)) != FR_OK){		//check again for the folder existence, ignoring the file name at the end of the path
					if (res == FR_NO_FILE){
						safe_printf("%sERROR: The destination file/folder does not exists%s\n",RED,WHITE);
						return res;
					}
					else{
						safe_printf("%sERROR checking existence of destination: %d%s\n",RED, res,WHITE);
						return res;
					}
				}
				else{
					folder_check = 1;														//**************************************check what this does.....
					//					safe_printf("%s FOLDER CHECK = 1\n %s, %s %s\n", YELLOW,new_exp, (*Arguments)[2],WHITE);
					//----------------------------------------------------do we have to safe new exp in (*Argument)[2]???????????????????????????
				}
			}
			else{
				safe_printf("%sERROR checking existence of destination: %d%s\n",RED, res,WHITE);
				return res;
			}
		}
		else{
			safe_printf("%sERROR checking existence of destination: %d%s\n",RED, res,WHITE);
			return res;
		}
	}
	if (fno.fattrib & AM_ARC){
		safe_printf("%sDestination is a file%s\n",YELLOW,WHITE);
		dest_file = 1;
	}
	else if (fno.fattrib & AM_DIR){
		safe_printf("%sDestination is a folder%s\n",YELLOW,WHITE);
		if (folder_check == 0){
			//			safe_printf("%sDestination is a folder%s\n",GREEN,WHITE);			//adds the source file name to the end of the path, if a file name has not been specified
			//			safe_printf("%s%s, %s %s\n", YELLOW,new_exp, (*Arguments)[2],WHITE);
			char forward_slash[2] = "/";
			strcat((*Arguments)[2],forward_slash);
			//			safe_printf("%sDestination folder: %s %s\n",PURPLE,(*Arguments)[2],WHITE);
			strcat((*Arguments)[2],(*Arguments)[1]);
			//			safe_printf("%sDestination folder: %s %s\n",PURPLE,(*Arguments)[2],WHITE);
		}
	}
	else {
		safe_printf("%sERROR: Destination is not a file or folder%s\n",RED,WHITE);
		return res;
	}
	//process
	//check if the destination file is the same as the current
	if (dest_file == 1){
		//		safe_printf("%s Destination = file, checking for same destination as source%s\n", YELLOW,WHITE);
		uint8_t path[BUFF_SIZE];
		if((res = f_getcwd(path, BUFF_SIZE)) != FR_OK){
			safe_printf("%sERROR finding the current directory: %d%s\n",RED,res,WHITE);
			return res;
		}
		//		else{
		//			safe_printf("%s> Current directory: %s%s\n",YELLOW, path,WHITE);
		//		}
		char ** word_array;		// output array of pointers to the start of each word
		word_array = (char * ) malloc (BUFF_SIZE * sizeof(char));
		if(!word_array)
					        return 6;
		int count;		//word count
		char character_to_search = '/';		// character that separates two words, in this case a forward slash
		count = string_parser((*Arguments)[2], &word_array,character_to_search);		//parse the string
		char *end_file = (word_array)[count-1];
		int exp_len = strlen(end_file);
		char expression[BUFF_SIZE] = {};
		if (end_file[exp_len-4] == '.'){		//check if the fourth last character is a '.', signifies a file extension
			//			safe_printf("\tExpression has an extension\n");
			char forward_slash[2] = "/";
			for(int i = 0; i < count-1; i++){
				strcat(expression,(word_array)[i]);
				//				safe_printf("%New expression: %s %s\n",PURPLE,expression,WHITE);			//add words to the path
				if ((i+1) != count-1){
					strcat(expression,forward_slash);										//add forward slashes to the path
					//					safe_printf("%c expression: %s %s\n",PURPLE,expression,WHITE);
				}
			}
			strcat(new_exp,expression);
			//			safe_printf("%sFinal destination: %s %s\n",PURPLE,new_exp,WHITE);
		}
		//		safe_printf("destination: %s\n", (*Arguments)[2]);
		if(strcasecmp(path,new_exp)==0){													//***************************what about new_exp???
			safe_printf("%sERROR: Current directory is the same as the destination\n%s",RED,WHITE);
			return res;
		}
		//		safe_printf("Current path and destination not the same\n");
	}
	//Open source file
	if ((res = f_open(&fsrc, (*Arguments)[1], FA_READ)) != FR_OK){
		safe_printf("%sError opening source file: %d%s\n",RED,res,WHITE);
		if(res == FR_NO_PATH){
			safe_printf("%sERROR: Could not find the path%s\n",RED,WHITE);
		}
		else if (res == FR_INVALID_NAME){
			safe_printf("%sERROR: The path name format is invalid%s\n",RED,WHITE);
		}
		else if (res == FR_INVALID_OBJECT){
			safe_printf("%sERROR: The file/directory object is invalid%s\n",RED,WHITE);
		}
		return res;
	}
	//Create destination file
	if ((res = f_open(&fdst, (*Arguments)[2], FA_WRITE | FA_CREATE_ALWAYS)) != FR_OK){
		safe_printf("%s\tError creating destination file: %d%s\n",RED,res,WHITE);
		if(res == FR_NO_PATH){
			safe_printf("%sERROR: Could not find the path%s\n",RED,WHITE);
		}
		else if (res == FR_INVALID_NAME){
			safe_printf("%sERROR: The path name format is invalid%s\n",RED,WHITE);
		}
		else if (res == FR_INVALID_OBJECT){
			safe_printf("%sERROR: The file/directory object is invalid%s\n",RED,WHITE);
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
	/*if (res = f_rename((*Arguments)[1],(*Arguments)[2]) != FR_OK){
		safe_printf("ERROR\n");
		safe_printf("\tError code: %d\n",res);
		return 1;
	}
	else{
		safe_printf("New folder: %s\n", (*Arguments)[1]);
	}*/
	return res;
}

FRESULT mkdir_function (unsigned int ArgNum, uint8_t **Arguments[]){
	FRESULT res;
#define BUFF_SIZE 256
	uint8_t rtext[BUFF_SIZE];
	if(ArgNum != 2){
		safe_printf("%sERROR: mkdir requires one argument only\n%s",RED,WHITE);
		return 6;
	}
	else if((res = f_mkdir((*Arguments)[1])) != FR_OK){
		safe_printf("%sERROR\n",RED);
		safe_printf("\tError code: %d\n%s",res,WHITE);
		return 1;
	}
	else{
		safe_printf("New folder: %s\n", (*Arguments)[1]);
	}
}

void debug_function(unsigned int ArgNum, uint8_t **Arguments[])
{
	if (ArgNum == 1)
	{
		// display debug
		if (get_debug() == 1)
		{
			safe_printf("Debug is currently on\n");
		}
		else
		{
			safe_printf("Debug is currently off\n");
		}
	}
	else if (strcmp(((*Arguments)[1]), "on") == 0)
	{
		// debug on entered
		safe_printf("Debug turned on \n");
		set_debug(1);
	}
	else if (strcmp(((*Arguments)[1]), "off") == 0)
	{
		// debug off entered
		safe_printf("Debug turned off \n");
		set_debug(0);
	}
	else
	{
		safe_printf("ERROR: debug requires correct arguments only\n");
	}
}

void speed_function(unsigned int ArgNum, uint8_t **Arguments[])
{
	if (ArgNum != 2)
	{
		safe_printf("ERROR: speed requires 2 arguments only\n");
	}
	else
	{
		int speed_number = atoi((*Arguments)[1]);
		if ((speed_number < 1) || (speed_number > 20))
		{
			safe_printf("ERROR: number entered is out of range\n");
		}
		else
		{
			safe_printf("Speed set to... \n");
			//set_speed(speed_number);
		}
	}
}

int* myReadFile(int *bytesread, char* read_file)
{
	//****************************************************************USE THIS TO GET THE VALUES AND PRINT ONCE IN THE LOAD STATE
#define READ_FILE "Hello.txt"
	static int* rtext;
	rtext = (int * ) malloc (BUFF_SIZE * sizeof(int));
	if(!rtext)
	        return NULL;
//	static int rtext[BUFF_SIZE];
	FRESULT res;
	//uint32_t bytesread;
	//********************************************************************************************dynamically allocate memory
	// Open file Hello.txt
	if((res = f_open(&MyFile, read_file, FA_READ)) != FR_OK)
	{
		safe_printf("%sERROR: Opening '%s'\n%s",RED, read_file,WHITE);
		safe_printf("\tError code: %d\n",res);
		return 1;
	}
	safe_printf("Task 1: Opened file '%s'\n", read_file);

	// Read data from file
	if ((res = f_read(&MyFile, rtext, 76, bytesread)) != FR_OK)
	{
		safe_printf("%sERROR: Reading '%s'\n%s",RED, read_file,WHITE);
		f_close(&MyFile);
		return 1;
	}
	//rtext[bytesread] = '\0';
	safe_printf("Bytes read: %d\n", (*bytesread));
	safe_printf("First element: %d\n", rtext[0]);
	for (int i = 0; i<(*bytesread)/sizeof(int);i++){
		safe_printf("%d\n", rtext[i]);
	}

	// Close file
	f_close(&MyFile);

	return rtext;
}

int myWriteFile(int values[], char* write_file, int length)		//do we need time column?
{
#define WRITE_FILE "There.txt"
	FRESULT res;
	UINT byteswritten = 0, bytesw;
	int values1[19] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
	for (int i = 0; i<length;i++){
		safe_printf("%d\n", values[i]);
	}
	// Open file There.txt
	if((res = f_open(&MyFile, write_file, FA_CREATE_ALWAYS | FA_WRITE)) != FR_OK)
	{
		safe_printf("%sERROR: Opening '%s'\n%s", RED,write_file,WHITE);
		return 1;
	}
	safe_printf("Task 1: Opened file '%s'\n", write_file);
	// Write to file
	if ((res = f_write(&MyFile, values, 76, &bytesw)) != FR_OK)
	{
		safe_printf("%sERROR: Writing '%s'\n%s",RED, write_file,WHITE);
		f_close(&MyFile);
		return 1;
	}
	byteswritten += bytesw;

	safe_printf("Task 1: Written: %d bytes\n", byteswritten);

	// Close file
	f_close(&MyFile);

	return 0;
}

void drawplay(int xpos, int ypos)
{
	// xpos, ypos top corner of play button
	int array_nos[21] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 19, 17, 15, 13, 11, 9, 7, 5, 3, 1};
	int new_y_pos = ypos;
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	for (int i = 0; i < 21; i++)
	{
		new_y_pos = ypos + i;
		BSP_LCD_DrawHLine(xpos, new_y_pos, array_nos[i]);
	}

}
void drawstop(int xpos, int ypos)
{
	// xpos, ypos top corner of play button
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_FillRect(xpos, ypos, 20, 20);		// xpos, ypos top corner of stop button
}
void drawsave(int xpos, int ypos)
{
	// xpos, ypos centre of save button
	BSP_LCD_SetTextColor(LCD_COLOR_ORANGE);
	BSP_LCD_FillRect(xpos-14, ypos-13, 28, 28);
	BSP_LCD_SetTextColor(LCD_COLOR_LIGHTGRAY);
	BSP_LCD_FillRect(xpos-9, ypos-14, 18, 14);
	BSP_LCD_FillRect(xpos-7, ypos+5, 14, 9);
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_DrawHLine(xpos-5, ypos+8, 10);
	BSP_LCD_DrawHLine(xpos-5, ypos+12, 10);
}
