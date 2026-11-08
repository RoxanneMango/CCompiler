#ifndef bla
#define bla
#endif

#define X true

#include <stdbool.h>
#include <stdio.h>

extern char NOWHERE;
const static unsigned long NUMBER = 1;

int
main(int argc, char ** argv)
{

	char testString[] =
	{
	"Hello w\
	orld! ho\
	w a\
	re you\
	 doi\
	ng?"
	};

	struct Hello
	{
		int a;
		char b;
	};
	
	typedef struct Hello Hello; 
	
	enum MagicNumbers
	{
		One = 1,
		Two = 2,
		Three = 3
	} magic;
		
	switch(magic)
	{
		case One: break;
		default: return -1;
	}
	
	volatile void * race[] = { "Boing boing ..." };
	
	for(register int i = 0; i < 10; i++);
	while(false);
	float x = 1.0;
	do{}while(x);
	
	printf("Hello world!\n");
	
	return 0;
}