#include "preprocessor.h"
#include "string_utils.h"
#include "syntax_tree.h"
#include "symbol_table.h"
#include "file_io.h"
#include "tokenization.h"
#include "preprocessor_directives.h"

#include <stdbool.h>
#include <stdlib.h>
#include <stddef.h>

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

int
preprocess(const char * filePATH, const char * outPATH)
{
	_SyntaxTree * syntaxTree_punctuation_and_operators = malloc(sizeof(_SyntaxTree));
	_initSyntaxTree(syntaxTree_punctuation_and_operators);

	_SyntaxTree * syntaxTree_keywords = malloc(sizeof(_SyntaxTree));
	_initSyntaxTree(syntaxTree_keywords);
	
	_LinkedStringList * _FileLines = malloc(sizeof(_LinkedStringList));
	initStringList(_FileLines);
	
	_Symbol_Table * _SymbolTable = malloc(sizeof(_Symbol_Table));
	initSymbolTable(_SymbolTable);

	readFromFile(filePATH, _FileLines);
	stitchTogether(_FileLines);
	filterOutComments(_FileLines);

	size_t numberOfElements = 0;	

	numberOfElements = sizeof(_Operators) / sizeof(_Operators[0]); 
	printf("\nOperators: ");
	for(int i = 0; i < numberOfElements; i++) 
	{
		printf("%s ", _Operators[i]);
		addSyntaxTreeNode(syntaxTree_punctuation_and_operators, _Operators[i], OperatorToken);
	}

	numberOfElements = sizeof(_Punctuation) / sizeof(_Punctuation[0]); 
	printf("\nPunctuation: ");
	for(int i = 0; i < numberOfElements; i++) 
	{
		printf("%s ", _Punctuation[i]);
		addSyntaxTreeNode(syntaxTree_punctuation_and_operators, _Punctuation[i], PunctuationToken);
	}

	printf("\nNumbers: 0 - 9");

	// identify primitive types and other keywords
	printf("\nControl flow: ");
	for(int i = 0; i < sizeof(_ControlFlow) / sizeof(_ControlFlow[0]); i++) 
	{
		printf("%s ", _ControlFlow[i]);
		addSyntaxTreeNode(syntaxTree_keywords, _ControlFlow[i], ControlFlowToken);
	}		
	printf("\nData Types: ");
	for(int i = 0; i < sizeof(_DataTypes) / sizeof(_DataTypes[0]); i++) 
	{
		printf("%s ", _DataTypes[i]);
		addSyntaxTreeNode(syntaxTree_keywords, _DataTypes[i], DataTypeToken);
	}	
	printf("\nStorage class: ");
	for(int i = 0; i < sizeof(_StorageClass) / sizeof(_StorageClass[0]); i++) 
	{
		printf("%s ", _StorageClass[i]);
		addSyntaxTreeNode(syntaxTree_keywords, _StorageClass[i], StorageClassToken);
	}
	printf("\nUser defined Types: ");
	for(int i = 0; i < sizeof(_UserDefinedTypes) / sizeof(_UserDefinedTypes[0]); i++) 
	{
		printf("%s ", _UserDefinedTypes[i]);
		addSyntaxTreeNode(syntaxTree_keywords, _UserDefinedTypes[i], UserDefinedTypeToken);
	}	
	printf("\nQualifiers: ");
	for(int i = 0; i < sizeof(_Qualifier) / sizeof(_Qualifier[0]); i++) 
	{
		printf("%s ", _Qualifier[i]);
		addSyntaxTreeNode(syntaxTree_keywords, _Qualifier[i], QualifierToken);
	}	
	numberOfElements = sizeof(_PreprocessorIdentifiers) / sizeof(_PreprocessorIdentifiers[0]); 
	printf("\nPreprocessor identifiers: ");
	for(int i = 0; i < numberOfElements; i++)
	{
		printf("%s ", _PreprocessorIdentifiers[i]);
		syntaxTree_PreprocessorDirectives.add(&syntaxTree_PreprocessorDirectives, _PreprocessorIdentifiers[i], PreprocessorToken);
	}
	printf("\n\n");

	_LinkedStringList * _Tokens = malloc(sizeof(_LinkedStringList));
	initStringList(_Tokens);

	// tokenization
	tokenization(_FileLines, _Tokens, syntaxTree_punctuation_and_operators);
	assignProcessedTokens(_Tokens, syntaxTree_keywords);
	
//	handlePreprocessorDirectives(_FileLines, syntaxTree, _SymbolTable);
//	expandIncludes(&_FileLines);
	
	_Tokens->print(_Tokens, false);
//	_FileLines->print(_FileLines, false);

	return 0;
}