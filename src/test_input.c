#ifndef bla
#define bla
#endif

#ifdef aaaa
#include <stdio.h>
#include <stdio.h>
#endif

#if x
#elif xx
#else
#endif

#define X true

#if (X)
#endif

#if X+U == true
#include "string_utils.h"
#include <string.h>
#else
#include <stdbool.h>
#endif

#define EVAL(X) (X)==0

// Definition of macros is checked before include expansion (include guards)
// include macros are expanded before defines!

struct _SyntaxTreeNode typedef _SyntaxTreeNode;
struct _SyntaxTree typedef _SyntaxTree;

char testString[] =
{
"Hello w\
orld! ho\
w a\
re you\
 doi\
ng?"
};

// scope for preprocessor:
//	- Replace all defines with their value
//  - Replace all includes with the actual code
//  - Remove all comments

/* Preprocessor phases:
*  1. Trigraph replacement: The preprocessor replaces trigraph sequences 
*	  with the characters they represent. This phase will be removed in 
*	  C23 following the steps of C++17.
*  2. Line splicing: Physical source lines that are continued with escaped 
*     newline sequences are spliced to form logical lines.
*  3. Tokenization: The preprocessor breaks the result into preprocessing 
*     tokens and whitespace. It replaces comments with whitespace.
*  4. Macro expansion and directive handling: Preprocessing directive lines, 
*     including file inclusion and conditional compilation, are executed. 
*	  The preprocessor simultaneously expands macros and, since the 1999 
*	  version of the C standard, handles _Pragma operators.
*/

int processToken(char * string, _LinkedStringList * tokenList, _SyntaxTree * syntaxTree);

static const int _StdHeaderLocationNum = 4;
char * _StandardLibraryHeaderLocations[] = 
{
	"/usr/lib/gcc/x86_64-linux-gnu/9/include",
	"/usr/local/include",
	"/usr/include/x86_64-linux-gnu",
	"/usr/include"
};

int
main(int argc, char ** argv)
{	
	int x = 4 * 20 / 5 + 10 - 3 % 2;
	
	if(x == 4 || x != 6 && !x)
	{
		x = 10, x^2;
	}
	
	return 0;
}