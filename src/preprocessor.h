#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

// Definition of macros is checked before include expansion (include guards)
// include macros are expanded before defines!

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

// null initialize member variables

/* define where preprocessor looks for the standard C library header files
*
* `gcc -print-prog-name=cpp` -v
*
* This command asks gcc which C preprocessor it is using, and then asks that 
* preprocessor where it looks for includes.
*
* #include "..." search starts here:
* ....
* #include <...> search starts here:
* /usr/lib/gcc/x86_64-linux-gnu/9/include
* /usr/local/include
* /usr/include/x86_64-linux-gnu
* /usr/include
* End of search list.
*/

int preprocess(const char * filePATH, const char * outPATH);

#endif // PREPROCESSOR_H