//	Assignment 1 Question 4
//	Joshua Beverley (c3195884) & Radhika Feron (c3256870)
//	Due 23/3/2018

#include "Ass-01.h"

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
	printf("\ncharacters: %d\nwords: %d\n", characters, no_of_words);

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
	(**array_of_words_p) = (char * ) malloc (no_of_words);	// mallocs the memory for array of words, so it can be stored outside this function
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

	printf("\n");
	return no_of_words;		// return the number of words in the string
}
