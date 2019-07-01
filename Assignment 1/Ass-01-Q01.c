//	Assignment 1 Question 1
//	Joshua Beverley (c3195884) & Radhika Feron (c3256870)
//	Due 23/3/2018

// Question 1

#include "Ass-01.h"

int read_coefficients(int *coeff_num_p, double **coeff_values_p, char *filename)
{

	FILE* fp; //pointer to a stream
	fp = fopen(filename,"rb"); //open a file for reading
	if (fp == NULL){		//if the file pointer returns NULL
		printf("ERROR: File open failed\n");
		fclose(fp);		//ensure the file is closed
		return -1;		//function returns -1 to indicate failure
	}

	int file_size;	//size of the entire file
	fseek(fp,0,SEEK_END);		//sets the file position to the end of the file
	file_size = ftell(fp);		//ftell returns the current file position and stores it in the file_size
	fseek(fp, 0, SEEK_SET);

	if (file_size < 4)		//if the file_size is less than 4, then the file does not match specified format
	{
		printf("ERROR: File size less than 4 bytes\n");		//print error
		fclose(fp);
		return -1;
	}
	if (fread(coeff_num_p, sizeof(int), 1, fp) != 1)		//read in the number of coefficients into coeff_num_p
	{	//if the condition is met
		printf("\nNumber of coefficients was not read successfully.");	//print error
		fclose(fp);		//close file
		return -1;		//function returns -1 to indicate failure
	}
	int coeff_bytes = 8*(*coeff_num_p);			//calculates the number of coefficients to be read
	if (coeff_bytes!=(file_size-4))	//if the number of bytes meant to read does not equal the number of data bytes in the file
	{
		printf("ERROR: File size does not meet specifications\n");		//print error
		fclose(fp);
		return -1;
	}

	double *coeff_val_temp;		//used to point to an array that will contain the coefficient data

	coeff_val_temp = (double*)malloc((*coeff_num_p)*sizeof(double));		//allocate the required space using malloc

	if(coeff_val_temp == NULL){			//if the coeff_val_temp returns null
		printf("ERROR: Could not allocate array\n");		//print error
		fclose(fp);
		return -1;
	}

	if (fread(coeff_val_temp, sizeof(double), *coeff_num_p, fp) != *coeff_num_p){
		printf("ERROR: the coefficient data was not successfully read");
		fclose(fp);
		return -1;
	}

	*coeff_values_p = &coeff_val_temp[0];		//let coeff_values_p point to the coefficient data array
	fclose(fp); //close the file

	return 0;		//to indicate success
}
