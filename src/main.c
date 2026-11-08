#include <stdio.h>

#include "preprocessor.h"

int
main(int argc, char ** argv)
{	
	if(argc > 1)
	{
		printf("argc: %d\n", argc);
		for(int i = 1; i < argc; i++)
		{
			printf("argv[%d] = %s\n", i, argv[i]);
		}
	}

	preprocess("test_input.c", "test_input.E");

	return 0;
}