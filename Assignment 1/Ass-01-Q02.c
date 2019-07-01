//	Assignment 1 Question 2
//	Joshua Beverley (c3195884) & Radhika Feron (c3256870)
//	Due 23/3/2018

// Question 2

#include "Ass-01.h"

int read_pcm_wavefile(pcm_wavefile_header_t *header_p, char **data_p, char *filename)
{
	char* RIFF_arp = "RIFF";	//string the ChunkID will be compared to (data integrity check)
	char* WAVE_arp = "WAVE";	//string the Format will be compared to (data integrity check)
	char* fmt_arp = "fmt";		//string the Subchunk1ID will be compared to (data integrity check)
	char* data_arp = "data";	//string the Subchunk2ID will be compared to (data integrity check)

	int test_ChunkSize,test_ByteRate,test_BlockAlign,test_Subchunk2Size;	//temporary variables to calculate the expected value of the header data (integrity check)
	int file_size;			//size of the entire file
	int i;					//used in for loops
	FILE* fp; 				//pointer to a stream

	fp = fopen(filename,"rb"); 		//open a file for reading
	if (fp==NULL){					//if the file pointer returns null
		printf("\nError: File open failed\n");	//the file failed to open, error message printed
		fclose(fp);	//close the file
		return -1;								//function returns -1 to indicate failure
	}

	fseek(fp,0,SEEK_END);		//sets the file position to the end of the file
	file_size = ftell(fp);			//ftell returns the current file position and stores it in the integer 'size'
	fseek(fp, 0, SEEK_SET);		//sets the file position to the beginning of the file

	if (file_size < 44){		//checks if there is enough data in the file for the header before reading it
		printf("\nError: Total file size (%d bytes) is less than the expected header size (44 bytes)\n", file_size);
		fclose(fp);		//close the file
		return -1;		//exit function, indicating failure
	}
	//The following section reads the header and stores the data in the corresponding variables within the pcm_wavefile_header_t structure
	//It follows the structure of PCM encoded WAVE files
	if (fread(&header_p->ChunkID, sizeof(uint8_t), 4, fp) != 4){ 	//read the data and check for the success of fread
		printf("\nError: ChunkID was not read properly\n");			//print error if fread was not successful
		fclose(fp);
		return -1;
	}
	for(i=0;i<4;i++){
		if (header_p->ChunkID[i] != RIFF_arp[i]){	//compare the ChunkID char by char to an array containing "RIFF"
			printf("\nError: ChunkID did not contain 'RIFF'\n");	//if the ChunkID character does not match the expected character, print error
			fclose(fp);
			return -1;
		}
	}
	if (fread(&header_p->ChunkSize, sizeof(uint32_t), 1, fp) != 1){
		printf("\nError: ChunkSize was not read properly\n");
		fclose(fp);
		return -1;
	}
	test_ChunkSize = file_size - sizeof(double);			//calculates the expected value of Chunksize
	if (test_ChunkSize != header_p->ChunkSize){		//if the calculated value does not match the read value
		printf("\nError: ChunkSize != size of file - 8 bytes (calculated %d)\n",test_ChunkSize);		//print error
		fclose(fp);
		return -1;
	}
	if (fread(&header_p->Format, sizeof(uint8_t), 4, fp) != 4){
		printf("\nError: Format was not read properly\n");
		fclose(fp);
		return -1;
	}
	for(i=0;i<4;i++){
		if (header_p->Format[i] != WAVE_arp[i]){		//compare the Format char by char to an array containing "WAVE"
			printf("\nError: Format did not contain 'WAVE'\n");		//if the Format character does not match the expected character, print error
			fclose(fp);
			return -1;
		}
	}
	if (fread(&header_p->Subchunk1ID, sizeof(uint8_t), 4, fp) != 4){
		printf("\nError: Subchunk1ID was not read properly\n");
		fclose(fp);
		return -1;
	}
	for(i=0;i<3;i++){
		if (header_p->Subchunk1ID[i] != fmt_arp[i]){				//compare the SubChunk1ID char by char to an array containing "fmt"
			printf("\nError: Subchunk1ID did not contain 'fmt'\n");		//if the SubChunk1ID character does not match the expected character, print error
			fclose(fp);
			return -1;
		}
	}
	if (fread(&header_p->Subchunk1Size, sizeof(uint32_t), 1, fp) != 1){
		printf("\nError: Subchunk1Size was not read properly\n");
		fclose(fp);
		return -1;
	}
	if (header_p->Subchunk1Size != 16){		//if the read value of Subchunk1size does not equal 16
		printf("\nError: Expecting Subchunk1Size = 16, possibly not PCM encoded\n");		//print error
		fclose(fp);
		return -1;
	}
	if (fread(&header_p->AudioFormat, sizeof(uint16_t), 1, fp) != 1){
		printf("\nError: AudioFormat was not read properly\n");
		fclose(fp);
		return -1;
	}
	if (header_p->AudioFormat != 1){		//if the AudioFormat variable does not equal 1
		printf("\nError: Expecting AudioFormat = 1, indicates some form of compression (not PCM)\n");	//print error
		fclose(fp);
		return -1;
	}
	if (fread(&header_p->NumChannels, sizeof(uint16_t), 1, fp) != 1){
		printf("\nError: NumChannels was not read properly\n");
		fclose(fp);
		return -1;
	}
	if ((header_p->NumChannels == 0) & (header_p->NumChannels != 1) & (header_p->NumChannels != 2)){		//if NumChannels equals zero
		printf("\nError: Channel is not mono (1) or stereo (2)\n");		//print error
		fclose(fp);
		return -1;
	}
	if (fread(&header_p->SampleRate, sizeof(uint32_t), 1, fp) !=1 ){
		printf("\nError: SampleRate was not read properly\n");
		fclose(fp);
		return -1;
	}
	if (header_p->SampleRate == 0){		//if the sample rate equals zero
		printf("\nError: Cannot have zero SampleRate\n");		//print error
		fclose(fp);
		return -1;
	}
	if (fread(&header_p->ByteRate, sizeof(uint32_t), 1, fp) != 1){
		printf("\nError: ByteRate was not read properly\n");
		fclose(fp);
		return -1;
	}
	if (fread(&header_p->BlockAlign, sizeof(uint16_t), 1, fp) != 1){
		printf("\nError: BlockAlign was not read properly\n");
		fclose(fp);
		return -1;
	}
	if (fread(&header_p->BitsPerSample, sizeof(uint16_t), 1, fp) != 1){
		printf("\nError: BitsPerSample was not read properly\n");
		fclose(fp);
		return -1;
	}
	if ((header_p->BitsPerSample == 0) & (header_p->BitsPerSample != 8) & (header_p->BitsPerSample != 16)){	//if Bitspersample is equal to 0, or other values apart from 8 and 16
		printf("\nError: BitsPerSample is not expected 8 or 16\n");		//print out error
		fclose(fp);
		return -1;
	}
	test_ByteRate = header_p->SampleRate * header_p->NumChannels * header_p->BitsPerSample/8;		//calculate the expected value of ByteRate
	if (test_ByteRate != header_p->ByteRate){	//if the calculated byterate is not equal to the read value
		printf("\nError: ByteRate != SampleRate * NumChannels * BitsPerSample/8 (calculated %d)\n", test_ByteRate);		//print error
		fclose(fp);
		return -1;
	}
	test_BlockAlign = header_p->NumChannels * header_p->BitsPerSample/8;		//calculate the expected value of BlockAlign
	if (test_BlockAlign != header_p->BlockAlign){		//if the calculated blockalign is not equal to the read value
			printf("\nError: BlockAlign != NumChannels * BitsPerSample/8 (calculated %d)\n", test_BlockAlign);		//print error
			fclose(fp);
			return -1;
	}
	if (fread(&header_p->Subchunk2ID, sizeof(uint8_t),4, fp) != 4){
		printf("\nError: Subchunk2ID was not read properly\n");
		fclose(fp);
		return -1;
	}
	for(i=0;i<4;i++){
			if (header_p->Subchunk2ID[i] != data_arp[i]){						//compare the Subchunk2ID char by char to an array containing "data"
				printf("\nError: Subchunk2ID did not contain 'data'\n");			//if the Subchunk2ID character does not match the expected character, print error
				fclose(fp);
				return -1;
			}
	}
	if (fread(&header_p->Subchunk2Size, sizeof(uint32_t), 1, fp) != 1){
		printf("\nError: Subchunk2Size was not read properly\n");
		fclose(fp);
		return -1;
	}
	test_Subchunk2Size = file_size - 44;			//calculate the expected value of Subchunk2Size
	if (header_p->Subchunk2Size % 2 != 0) {		//if Subchunk2Size is an odd number
		// test_Subchunk2Size is odd - has added padded byte
		if (test_Subchunk2Size - 1 == header_p->Subchunk2Size){		//if the calculated value of Subchunk2size minus 1 is equal to the file value
			printf("\nWarning: Odd number of samples, so file padded by one byte (no error)");	//print information
		}
		else{
			printf("\nError: Subchunk2Size != size - 44 (calculated %d)\n", test_Subchunk2Size);		//else print error
			fclose(fp);
			return -1;
		}
	}
	//test_Subchunk2Size is even
	else if (test_Subchunk2Size != header_p->Subchunk2Size){		//if the calculate value is not equal to the file value
		printf("\nError: Subchunk2Size != size - 44 (calculated %d)\n", test_Subchunk2Size);		//print error
		fclose(fp);
		return -1;
	}
	//if there are no errors in the file, print the information
	printf("\n\tChunkID: %c%c%c%c",header_p->ChunkID[0],header_p->ChunkID[1],header_p->ChunkID[2],header_p->ChunkID[3]);
	printf("\n\tChunkSize: %d", header_p->ChunkSize);
	printf("\n\tFormat: %c%c%c%c", header_p->Format[0],header_p->Format[1],header_p->Format[2],header_p->Format[3]);
	printf("\n\tSubChunk1ID: %c%c%c", header_p->Subchunk1ID[0],header_p->Subchunk1ID[1],header_p->Subchunk1ID[2]);
	printf("\n\tSubchunk1Size: %d", header_p->Subchunk1Size);
	printf("\n\tAudioFormat: %d", header_p->AudioFormat);
	printf("\n\tNumChannels: %d", header_p->NumChannels);
	printf("\n\tSampleRate: %d", header_p->SampleRate);
	printf("\n\tByteRate: %d", header_p->ByteRate);
	printf("\n\tBlockAlign: %d", header_p->BlockAlign);
	printf("\n\tBitsPerSample: %d", header_p->BitsPerSample);
	printf("\n\tSubchunk2ID: %c%c%c%c", header_p->Subchunk2ID[0],header_p->Subchunk2ID[1],header_p->Subchunk2ID[2],header_p->Subchunk2ID[3]);
	printf("\n\tSubchunk2Size: %d", header_p->Subchunk2Size);

	//Read the data
	printf("\nReading data... \n");
	char *data_temp;		//create a pointer to a char variable

	data_temp = (char*)malloc((header_p->Subchunk2Size)*sizeof(char));		//allocate memory

	if(data_temp == NULL){		//if the data_temp point returns null
		printf("Could not allocate array\n");		//print error
		fclose(fp);		//close the file
		return -1;		//return -1 to indicate failure
	}

	if (fread(data_temp, sizeof(char), header_p->Subchunk2Size, fp) != header_p->Subchunk2Size){
		printf("Error: Data chunk not read successfully\n");		//read all the data into the data_temp variable
		fclose(fp);
		return -1;
	}

	*data_p = &data_temp[0];		//let the data_p pointer point to the data just read

	  fclose(fp);		//close file
	  return 0;			//function returns 0 to indicate success
}

int write_pcm_wavefile(pcm_wavefile_header_t *header_p, char *data, char *filename)
{
	//assuming there are no errors in the header/data since they have been checked in the read_pcm_wavefile function
	FILE* fp; //pointer to a stream
	fp = fopen(filename,"wb"); //open a file for writing
	if (fp == 0){		//if the file pointer returns NULL
		printf("\nERROR: File open failed\n");
		fclose(fp);		//close the file
		return -1;		//function returns -1 to indicate failure
	}

	if (fwrite(header_p, 44, 1, fp) != 1){
		printf("Error in writing the header.\n");		//write the header information to the file
		fclose(fp);
		return -1;
	}
	if (fwrite(data, header_p->Subchunk2Size, 1, fp) != 1){
		printf("Error in writing the data.\n");		//write the data information to the file
		fclose(fp);
		return -1;
	}
	if (header_p->Subchunk2Size % 2 != 0){ //odd data size
		int pad_data = 0;
		if (fwrite(&pad_data, sizeof(char), 1, fp) != 1){
				printf("Error in writing the padded byte.\n");		//write the data information to the file
				fclose(fp);
				return -1;
			}
	}
	int file_size;				//size of the entire file
	fseek(fp,0,SEEK_END);		//sets the file position to the end of the file
	file_size = ftell(fp);		//ftell returns the current file position and stores it in the integer 'file_size'
	printf("\nSize of written file: %d", file_size);

	int data_size = header_p->ChunkSize + 8 ;		//calculates the size of the written data
	if (data_size == file_size){		//if the data size matches the file size
		printf("\nFile size matches data size. File successfully written.\n");		//print success message
	}
	else{
		printf("\nFile size does not match data size. File unsuccessfully written.\n");		//else print error message
		fclose(fp);		//close the file
		return -1;		//return -1 to indicate failure
	}
  fclose(fp); //close the file
  return 0;	//function returns 0 to indicate success
}
