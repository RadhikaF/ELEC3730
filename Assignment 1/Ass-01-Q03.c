//	Assignment 1 Question 3
//	Joshua Beverley (c3195884) & Radhika Feron (c3256870)
//	Due 23/3/2018

#include "Ass-01.h"

int filter(char *filter_filename, char *input_wavefilename, char *output_wavefilename)
{
	  //Finite impulse response FIR filter
	  //filtering action described by coefficients (Q1)
	  //read from input_wavefilename, write to output_wavefilename
	  //coefficients for filter in filter_name

	  pcm_wavefile_header_t header;
	  char *data;
	  if (read_pcm_wavefile(&header, &data, input_wavefilename)==-1){
		  printf("Error: File was not successfully read\n");
		  return -1;
	  }
	  int coeff_num;
	  double *coeff_values; // Array of coefficient values
	  if (read_coefficients(&coeff_num, &coeff_values, filter_filename)!=0)
	  {
		printf("Error: Filter file not successfully read\n");
		return -1;
	  }

	  // convert 8bit to 16 bit
	  int data_size = header.Subchunk2Size;		// data_size is the size of the data in 8 bit form
	  printf("Number of Bytes = %d\n", data_size);
	  int new_data[data_size / 2];		// array used to store the new data, in 16 bit form
	  int filtered_data [data_size / 2];	// array used to store the filtered data in 16 bit form
	  //char * new_data = (char *) malloc((data_size + 2) / 2);
	  int lsb, msb, total;		// define the least sig byte, most sig byte and total variables used to make 16 bit numbers
	  int counter = 0; 		// counter used in the for loop, counts number of 16 bit numbers
	  int datacounter = 0;	// counter used in the for loop, counts number of 8 bit numbers
	  for (counter = 0; ((datacounter) <= (data_size - 1)); counter++)
	  {
		  // repeat until datacounter reaches number of data points (8 bit)
		  datacounter = counter * 2;		// update value of datacounter, counter was updated in the for loop
		  lsb = data[datacounter];		// least sig byte set to the first byte in the data array (first in a group of 2)
		  if ((data_size % 2 == 1) && (datacounter == (data_size - 1)))
		  {
			  msb = 0;		// if there is an odd number of data points in the array,
			  	  	  	  	// and this is the last data point being read, assume the MSB is 0
		  }
		  else
		  {
			  msb = data[datacounter + 1];	// otherwise, assign the second in a group of 2 bytes to most significant byte
		  }
		  msb = msb << 8;		// bit shift the most significant byte
		  total = msb + lsb;	// add the bit shifted msb and the lsb to create a 16 bit data number
		  new_data[counter] = total;	// set this data number to the new data set
	  }

	  // filter the data
	  //counter--;
	  int upcounter = 0;	// initialise a new counter, used to count the number of 16 bit numbers up from 0
	  int internal_counter = 0;		// counter used to move through the coefficients for the filtering
	  double sum; 		// initialise sum, the total used to implement the filter
	  for (upcounter = 0; upcounter < counter; upcounter++)	// repeats for all elements in the new data array
	  {
		  internal_counter = 0;	// reset the internal counter
		  sum = 0; 		// resets the sum
		  for (internal_counter = 0; internal_counter < coeff_num; internal_counter++)	// repeats for all coefficients
		  {
			  if ((upcounter + internal_counter) < counter)
			  {
				  // if the total of the two counters exceeds the bounds of the data array, ignore remaining coefficients
				  // otherwise, multiply the coefficient with the data element and sum with the rest of the data
				  sum = coeff_values[internal_counter] * new_data[upcounter + internal_counter + 1] + sum;
			  }
		  }
		  sum += 0.5;	// add 0.5 to the sum to account for type casting truncation
		  filtered_data[upcounter] = (int)sum; 		// convert the sum to an integer data type and save in the filtered data array
	  }

	  // convert 16bit to 8bit
	  upcounter = 0;	// reset the upcounter
	  for (upcounter = 0; upcounter < counter; upcounter++) 	// repeats for all elements in the filtered data array
	  {
		  total = filtered_data[upcounter];	// assigns the filtered data point to total
		  msb = total / 256;		// finds the most significant byte from total
		  lsb = total - (msb * 256);		// using total and msb, finds the least significant byte
		  data[upcounter * 2] = lsb;	// saves the least and most significant bytes back into the data array
		  data[upcounter * 2 + 1] = msb;
	  }

	  // write the file
	 if ((write_pcm_wavefile(&header, data, output_wavefilename)) == -1)
	  {
		  printf("Error: File was not successfully written. ");
		  return -1;
	  }

	  return 1;
}
