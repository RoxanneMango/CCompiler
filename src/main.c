#include <stdio.h>
#include <string.h>

#include "preprocessor.h"

int
main(int argc, char ** argv)
{	
	char fileName[255]; memset(fileName, 0, 255);
	if(argc > 1)
	{
		printf("argc: %d\n", argc);
		for(int i = 1; i < argc; i++)
		{
			printf("argv[%d] = %s\n", i, argv[i]);
		}
		strcpy(fileName, argv[1]);		
	}
	else
	{
		strcpy(fileName, "test_input");		
	}

	char file_in[255];			char file_out[255];
	memset(file_in, 0, 255); 	memset(file_out, 0, 255);
	strcpy(file_in , fileName); strcpy(file_out, fileName);
	strcat(file_in , ".c"); 	strcat(file_out, ".E");

	preprocess(file_in, file_out);

	return 0;
}