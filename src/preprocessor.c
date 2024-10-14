#ifndef bla
#define bla
#endif

// terminal colors: https://www.codeproject.com/Articles/5329247/How-to-Change-Text-Color-in-a-Linux-Terminal

#ifdef aaaa
#include <stdio.h>
#include <stdio.h>
#include <stdio.h>
#include <stdio.h>
#include <stdio.h>
#endif

#if x
#elif xx
#else
#endif

#define X true

#if X+U == true
#include "string_utils.h"
#include <string.h>
#else
#include <stdbool.h>
#endif

#include <stdlib.h>	// standard memory operations, etc
#include <stdio.h>  // standard IO
//#include <string.h>	// string manipulations
//#include <stdbool.h>// true false bool definition
#include <dirent.h> // DIR * definition

#if (1 == 0)
#define Y true
#endif

#if Y == true
#undef Y
#ifdef Y
exit(-1);
printf("Hello world!\n");
int x = 1234;
#endif
#endif

#include "symbol_table.h"

#define EVAL(X) (X)==0

#if hoooo
#ifndef bla
#define bla
#define bla
#if true
#elif true
#endif
#endif
#endif

#if defined (bla)
#undef bla
#endif

#ifdef bla
#undef bla
#endif

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

static const int _StdHeaderLocationNum = 4;
char * _StandardLibraryHeaderLocations[] = 
{
	"/usr/lib/gcc/x86_64-linux-gnu/9/include",
	"/usr/local/include",
	"/usr/include/x86_64-linux-gnu",
	"/usr/include"
};

const char * LINE_MACRO = "__LINE__";
const char * FILE_MACRO = "__FILE__";

int
filterOutComments(_LinkedStringList * list)
{
	DEBUG_PRINT("filterOutComments() ...\n");

	if(!list)
	{
		DEBUG_PRINT("_LinkedStringList * was NULL\n");
		return -1;
	}
	if(!list->head)
	{
		DEBUG_PRINT("_LinkedStringList head was NULL\n");
		return -1;		
	}

	DEBUG_PRINT("file length: %d\n", list->length);
	
	bool isCommentBlock = false;
	
	_StringNode * node = list->head;
	while(node)
	{
		char * line = node->data;
		
		// filter out comments
		char * commentStart = strstr(line, "//");
		char * blockCommentStart = strstr(line, "/*");
		if(commentStart)
		{
			// if its at index 0, ignore the rest of the line, its definitely a comment
			if(line != commentStart)
			{
				// get starting position of comment
				int commentStartPos = abs(line - commentStart);
				
				// if not part of string, remove comment
				if(!partOfString(line, commentStart))
				{
					line[commentStartPos] = ' ';
					line[commentStartPos+1] = '\n';
					line[commentStartPos+2] = '\0';
				}
				// remove line if its only whitespace anyway ...
				if(isOnlyWhiteSpaces(line))
				{
					removeStringNode(list, node);
				}
			}
			else
			{
				removeStringNode(list, node);
			}
		}
		else if(blockCommentStart)
		{
			if(!partOfString(line, blockCommentStart))
			{
				isCommentBlock = true;
				char * blockCommentEnd = strstr(line, "*/");
				int blockCommentStartPos = abs(line - blockCommentStart);
				
				// inline comment
				if(blockCommentEnd)
				{
					int blockCommentEndPos = abs(line - blockCommentEnd) + 2;
					DEBUG_PRINT("inline comment\n");
					isCommentBlock = false;

					DEBUG_PRINT("a: %d ; b: %d\n", blockCommentStartPos, blockCommentEndPos);
					
					char split1[strlen(line)]; split1[0] = '\0';
					char split2[strlen(line)]; split2[0] = '\0';
					
					strncat(split1, line, blockCommentStartPos);
					strncat(split2, (line+blockCommentEndPos), strlen(line)-blockCommentEndPos);
					
					DEBUG_PRINT("split1: %s ; split2: %s\n", split1, split2);
										
					char splitstitch[strlen(line)]; splitstitch[0] = '\0';
					strcat(splitstitch, split1);
					strcat(splitstitch, split2);

					DEBUG_PRINT("splitstitch: %s\n", splitstitch);
					
					splitstitch[blockCommentEndPos] = ' ';
					splitstitch[blockCommentEndPos+1] = '\0';
					
					if(isOnlyWhiteSpaces(splitstitch))
					{
						removeStringNode(list, node);
					}
					else
					{
						line[0] = '\0';
						strcat(line, splitstitch);
					}
				}
				else
				{
					line[blockCommentStartPos] = '\0';
					if( (blockCommentStart == line) || 
						isOnlyWhiteSpaces(line) )
					{
						removeStringNode(list, node);
					}
				}
			}
		}
		else if(isCommentBlock)
		{
			char * blockCommentEnd = strstr(line, "*/");
			if(blockCommentEnd)
			{
				isCommentBlock = false;
				int isCommentBlockPos = abs(line - blockCommentEnd) + 2;
				if(isOnlyWhiteSpaces(line+isCommentBlockPos))
				{
					removeStringNode(list, node);
				}
				else
				{
					line = line + isCommentBlockPos;
				}
			}
			else
			{
				removeStringNode(list, node);
			}
		}
		else
		{
			if(isOnlyWhiteSpaces(line))
			{
				removeStringNode(list, node);
			}
		}
		node = node->next;
	}	
	return 0;
}

int
filterOutCommentsFromFile(const char * filePATH, const char * outPATH)
{
	DEBUG_PRINT("filterOutComments() ...\n");

	FILE * FP = fopen(filePATH, "r");
	FILE * FP_OUT = fopen(outPATH, "w");
	
	if(!FP)
	{
		return -1;
	}
	if(!FP_OUT)
	{
		return -1;
	}
	
	char chunk[128];
	
	size_t len = sizeof(chunk);
	char * line = malloc(len);
	
	if(line == NULL)
	{
		return -1;
	}
		
	bool isCommentBlock = false;
	
	while(fgets(chunk, sizeof(chunk), FP) != NULL)
	{
		// empty the line buffer
		line[0] = '\0';
		
		// Resize the line buffer if necessary
		size_t len_used = strlen(line);
		size_t chunk_used = strlen(chunk);

		if(len - len_used < chunk_used)
		{
			len *= 2;
			if((line = realloc(line, len)) == NULL)
			{
				printf("Unable to reallocate memory for the line buffer\n");
				free(line);
				return -1;
			}
		}

		// Copy the chunk to the end of the line buffer
		strncpy(line + len_used, chunk, len - len_used);
		len_used += chunk_used;

		// Check if line contains '\n', if yes process the line of text
		// the preprocessing happens here ...
		if(line[len_used - 1] == '\n')
		{			
			// filter out comments
			char * substr = strstr(line, "//");
			char * substr2 = strstr(line, "/*");
			if(substr)
			{
				// if its at index 0, ignore the rest of the line, its definitely a comment
				if(line != substr)
				{
					// get starting position of comment
					int a = abs(line - substr);
					
					// if not part of string, remove comment
					if(!partOfString(line, substr))
					{
						line[a] = ' ';
						line[a+1] = '\n';
						line[a+2] = '\0';
					}
					if(!isOnlyWhiteSpaces(line))
					{
						fputs(line, FP_OUT);
					}
				}
			}
			else if(substr2)
			{
				if(partOfString(line, substr2))
				{
					fputs(line, FP_OUT);
				}
				else
				{					
					isCommentBlock = true;
					char * substr3 = strstr(line, "*/");
					
					// inline comment
					if(substr3)
					{
						DEBUG_PRINT("inline comment\n");
						isCommentBlock = false;
						int a = abs(line - substr2);
						int b = abs(line - substr3) + 2;
						DEBUG_PRINT("a: %d ; b: %d\n", a, b);
						
						char split1[strlen(line)]; split1[0] = '\0';
						char split2[strlen(line)]; split2[0] = '\0';
						
						strncat(split1, line, a);
						strncat(split2, (line+b), strlen(line)-b);
						
						DEBUG_PRINT("split1: %s ; split2: %s\n", split1, split2);
											
						char splitstitch[strlen(line)]; splitstitch[0] = '\0';
						strcat(splitstitch, split1);
						strcat(splitstitch, split2);

						DEBUG_PRINT("splitstitch: %s\n", splitstitch);
						
						splitstitch[b] = ' ';
						splitstitch[b+1] = '\0';
						
						if(!isOnlyWhiteSpaces(splitstitch))
						{
							fputs(splitstitch, FP_OUT);
						}
					}
				}
			}
			else if(isCommentBlock)
			{
				char * substr3 = strstr(line, "*/");
				if(substr3)
				{
					isCommentBlock = false;
					int b = abs(line - substr3) + 2;
					if(!isOnlyWhiteSpaces(line+b))
					{
						fputs((line+b), FP_OUT);
					}
				}
			}
			else
			{
				if(!isOnlyWhiteSpaces(line))
				{
					fputs(line, FP_OUT);
				}
			}
		}
	}

	fclose(FP);
	fclose(FP_OUT);

	free(line);
	
	return 0;
}

int
readFromFile(const char * filePATH, _LinkedStringList * fileList)
{	
	// parse the file before processing it
	FILE * FP = fopen(filePATH, "r");
	
	if(!FP)
	{
		return -1;
	}
	
	char chunk[128];
	
	size_t len = sizeof(chunk);
	char * line = malloc(len);
	
	if(line == NULL)
	{
		return -1;
	}
	
	// empty the line buffer
	line[0] = '\0';
	while(fgets(chunk, sizeof(chunk), FP) != NULL)
	{		
		// Resize the line buffer if necessary
		size_t len_used = strlen(line);
		size_t chunk_used = strlen(chunk);

		if(len - len_used < chunk_used)
		{
			len *= 2;
			if((line = realloc(line, len)) == NULL)
			{
				printf("Unable to reallocate memory for the line buffer\n");
				free(line);
				return -1;
			}
		}

		// Copy the chunk to the end of the line buffer
		strncpy(line + len_used, chunk, len - len_used);
		len_used += chunk_used;

		// Check if line contains '\n', if yes process the line of text
		if( (line[len_used - 1] == '\n') )
		{
			fileList->addNode(fileList, line, 0);
			// empty the line buffer
			line[0] = '\0';
		}
	}
	
	// add the last line
	if(line && strlen(line))
	{
		fileList->addNode(fileList, line, 0);
	}
	
	fclose(FP);
	free(line);
	
	return 0;
}

bool 
fileExistsInDirectory(const char * PATH, const char * file)
{
	DIR * DP = opendir(PATH);
	if(!DP)
	{
		DEBUG_PRINT("Could not open directory @ %s\n", PATH);
		return -1;
	}
	
	struct dirent * dir;
	bool fileExists = false;
	while((dir = readdir(DP)) && !fileExists)
	{
//		printf("d_name: %s ; file: %s\n", dir->d_name, file);
		if(strcmp(dir->d_name, file) == 0)
		{
			fileExists = true;
		}
	}
	closedir(DP);

	return fileExists;
}

int
getFilesFromDirectory(_LinkedStringList * list, const char * PATH)
{	
	DIR * DP = opendir(PATH);
	if(!DP)
	{
		DEBUG_PRINT("Could not open directory @ %s\n", PATH);
		return -1;
	}
	
	struct dirent * dir;
	while((dir = readdir(DP)))
	{
		if(strcmp(dir->d_name, ".") && strcmp(dir->d_name, ".."))
		{
			list->addNode(list, dir->d_name, 0);
//			printf("-> %s\n", dir->d_name);
		}
	}
	closedir(DP);

	return 0;
}

int 
validateInclude(const char * fileName)
{
	// check for what kind of includes;
	int fileNameLen = strlen(fileName);
	if(fileNameLen < 2)
	{
		return -1;
	}
	
	// check if it is valid
	if( ((fileName[0] == '<') && (fileName[fileNameLen-1] == '>')) ||
		((fileName[0] == '\"') && (fileName[fileNameLen-1] == '\"')) )
	{
		bool isFileFound = false;
		bool isLocalInclude = (fileName[0] == '\"');
		
		// remove first and last character to sanitize file name
		
		char file[strlen(fileName)-2]; file[0] = '\0';
		strncat(file, (fileName+1), strlen(fileName));
		file[strlen(file)-1] = '\0';
		
		// check if . directory needs to be included in the search path ...
		if(isLocalInclude)
		{
			if(fileExistsInDirectory(".", file))
			{
				isFileFound = true;
				DEBUG_PRINT("Local file exists: %s/%s\n", ".", file);
			}
		}
		// search through standard library headers
		if(!isFileFound)
		{
			for(int i = 0; (i < _StdHeaderLocationNum) && !isFileFound; i++)
			{
				if(fileExistsInDirectory(_StandardLibraryHeaderLocations[i], file))
				{
					DEBUG_PRINT("File exists: %s/%s\n", _StandardLibraryHeaderLocations[i], file);
					isFileFound = true;
				}
			}
		}
		// could not find file at all!
		if(!isFileFound)
		{
			DEBUG_PRINT("Could not find #included file %s\n", fileName);
			return 0;
		}
	}
	else
	{
		DEBUG_PRINT("Invalid include syntax!\n");
		return 0;
	}
	
	// final pass to check for illigal characters
	int forbiddenLen = 9;
	char * forbiddenCharacters[] = { "\\", "/", ":", "*", "?", "\"", "<", ">", "|" };
	for(int i = 0; i > forbiddenLen; i++)
	{
		if(strstr(fileName, forbiddenCharacters[i]))
		{
			return 0;
		}
	}
	
	return 1;
}

int
removeBetweenSymbols(_Symbol_Table * list, _LinkedStringList * fileList, _Symbol * begin)
{
	_Symbol * end = begin->next;
	int nesting = 0;
	while(end)
	{
		if(	!strcmp(end->name, "#if") ||
			!strcmp(end->name, "#ifdef") ||
			!strcmp(end->name, "#ifndef") )
		{
			nesting++;
		}
		if(strcmp(end->name, "#endif") == 0)
		{
			if(nesting == 0)
			{
				break;
			}
			nesting--;						
		}
		end = end->next;
	}
	DEBUG_PRINT("BEGIN = %s: %d\nEND = %s: %d\n", begin->filePosition->data, begin->filePosition->index, end->filePosition->data, end->filePosition->index);				
	
	_StringNode * a = begin->filePosition;
	_StringNode * b = end->filePosition;
	_StringNode * line = a;

	DEBUG_PRINT("Removing symbols from symbol table:\n");
	while(true)
	{
		DEBUG_PRINT("\tline: %s\n", line->data);
		_StringNode * next = line->next;
		fileList->removeNode(fileList, line);
		line = next;
		if(line == b)
		{
			DEBUG_PRINT("\tlast line: %s\n", line->data);
			fileList->removeNode(fileList, b);
			break;
		}
	}
//				list->removeNode(list, directive);
	DEBUG_PRINT("before directives length: %d\n", list->length);
	list->removeNode(list, begin);
	list->removeNode(list, end);
	DEBUG_PRINT("after directives length: %d\n", list->length);
	DEBUG_PRINT("Recursing back into handleIfGroups\n");
	
	return true;
}

// return the node where the preprocessing will continue
// return NULL if something went wrong during processing
void
handleIfGroups(_Symbol_Table * list, _LinkedStringList * fileList, _Symbol_Table * symbolTable, _Symbol * directive)
{
	int ifndef = false;
	while(directive)
	{
		char * name = directive->name;
		char * value = directive->value;
		
		if((ifndef = (strcmp(name, "#ifndef") == 0)) || (strcmp(name, "#ifdef") == 0))
		{
			DEBUG_PRINT("%s token!\n", ifndef ? "#ifndef" : "#ifdef");						

			// look through symbol table to see if symbol has been defined already
			_Symbol * foundSymbol = symbolTable->find(symbolTable, value);
			bool definedBeforeStatement = false;

			// symbol was defined at some point, but need to verify that it was defined before ifndef statement
			if(foundSymbol && directive->filePosition)
			{
				definedBeforeStatement =  directive->filePosition->index > foundSymbol->filePosition->index;
				DEBUG_PRINT("(%s: %d) > (%s: %d) = %d\n", directive->filePosition->data, directive->filePosition->index, foundSymbol->filePosition->data, foundSymbol->filePosition->index, definedBeforeStatement);
			}
			// execute
			if((ifndef && !definedBeforeStatement) || (!ifndef && definedBeforeStatement))
			{
				// recurse into it
				DEBUG_PRINT("token was%sdefined %s -- Executing segment!\n", ifndef ? "not" : " ", value);
			}
			// not execute
			else
			{
				DEBUG_PRINT("token was already defined: %s -- removing nodes!\n", value);
				removeBetweenSymbols(list, fileList, directive);
				handleIfGroups(list, fileList, symbolTable, list->head);
			}
		}
		else if(strcmp(name, "#if") == 0)
		{																		
			// handle if statement here ...
			DEBUG_PRINT("If token! : %s: %d\n", value, EVAL(value));				
		}
		
		directive = directive->next;
	}
}

int handleIncludeDirective(_LinkedStringList * tokenList)
{
	if(!tokenList)
	{
		DEBUG_PRINT("_LinkedStringList * is NULL\n");
		return -1;
	}
	if(tokenList->length < 2)
	{
		DEBUG_PRINT("Not enough tokens in _LinkedStringList *\n");
		return -1;
	}
	
	_StringNode * node = tokenList->head;
	// go to the values part
	if(node) node = node->next;
	if(node) node = node->next;
	
	while(node)
	{
		printf("Node: %s\n", node->data);
		node = node->next;
	}
	
	return 0;
}

int
evaluateExpression(_LinkedStringList * tokenList, _SyntaxTree * syntaxTree, _Symbol_Table * symbolTable)
{	
	// error checking
	if(!tokenList)
	{	DEBUG_PRINT("_LinkedStringList * was NULL\n"); 
		return -1; 
	}
	if(!tokenList->head) 
	{	DEBUG_PRINT("_LinkedStringList head was NULL\n"); 
		return -1;
	}
	if(!syntaxTree)
	{	DEBUG_PRINT("_SyntaxTree * was NULL\n");
		return -1;
	}
	if(!symbolTable)
	{	DEBUG_PRINT("_Symbol_Table * was NULL\n");
		return -1;
	}

	bool isDefinedKeyword = false;
	bool isValidState = true;
	bool isBraceOpened = false;
	
	int braceEncapsulationLevel = 0;

	_StringNode * node = tokenList->head;
	printf("Node > ");
	tokenList->print(tokenList, false);
	while(node)
	{
		if(node->type == IdentifierToken)
		{
			// find in symbolTable
			if(!strcmp(node->data, "defined"))
			{
				// find identifier or open brace punctuation
				node = node->next;
				if(!node) return -1;

				bool hasBraceOpen = false;
				if(!strcmp(node->data, "("))
				{
					hasBraceOpen = true;
					node = node->next;
					if(!node) return -1;
				}
			
				_Symbol * symbol = symbolTable->find(symbolTable, node->data);
				if(symbol)
				{
					printf("symbol was found: %s : %s\n", symbol->name, symbol->value ? symbol->value : "false");
				}
				else
				{
					printf("symbol not found! (%s)\n", node->data);
				}
				
				if(hasBraceOpen)
				{
					node = node->next;
					if(!node) return -1;
					if(strcmp(node->data, ")"))
					{
						DEBUG_PRINT("There was no closing brace!");
						return -1;
					}
				}
			}
		}
		else if(node->type == PunctuationToken)
		{

		}
		else if(node->type == NumberToken)
		{
			
		}
		node = node->next;
	}
	
	if(braceEncapsulationLevel != 0)
	{
		printf("Unequal amount of braces!");
	}
//	printf("\n");
		
	return 0;
}

int 
handlePreprocessorDirectives(_LinkedStringList * fileList, _SyntaxTree * syntaxTree, _Symbol_Table * symbolTable)
{
	if(!fileList)
	{	DEBUG_PRINT("_LinkedStringList * was NULL\n");
		return -1;
	}
	if(!fileList->head)
	{	DEBUG_PRINT("_LinkedStringList head was NULL\n");
		return -1;		
	}
	if(!symbolTable)
	{	DEBUG_PRINT("_Symbol_Table * was NULL\n");
		return -1;
	}
	if(!syntaxTree)
	{	DEBUG_PRINT("_SyntaxTree * was NULL\n");
		return -1;
	}

	int _LookingForEndif = 0;
	int _ElseEncountered = 0;
	int _IfEncountered = 0;
	
	int isSkip = 0;
	
	int _IfNum = 0;
	int _EndifNum = 0;
	int tokenNum = 0;
	
	// initialize the #if statement stack to push and pop to
//	_Symbol_Table * ifStack = malloc(sizeof(_Symbol_Table));
//	symbolTable->ifStack = malloc(sizeof(_Symbol_Table));
//	initSymbolTable(ifStack);
//	_Symbol_Table * ifStack = symbolTable->ifStack;
	
	_StringNode * node = fileList->head;
	
	_Symbol_Table directiveList;
	initSymbolTable(&directiveList);
	
	// static if statement analysis and adding preprocessor directives to one list for
	// further processing later
	while(node)
	{
		char * line = node->data;
		
		if(line[0] == '#' && (strlen(line) > 1))
		{
			tokenNum++;
			
			_LinkedStringList tokenList;
			initStringList(&tokenList);
			processToken(line, &tokenList, syntaxTree);
			
			// skip '#' 'directive'
			_StringNode * token = tokenList.head->next;
			char * directive = token->data;

			token = token ? token->next : NULL;
			char * name = token && token->data ? token->data : NULL;

			token = token ? token->next : NULL;
			char * value = token && token ? token->data : NULL;

//			printf("tokenlist print:\n");
//			tokenList.print(&tokenList, false);

//			printf("\ntoken:\n");
//			printf("directive: %s;\n", directive);
//			printf("name: %s;\n", name);
//			printf("value: %s\n", value);
			
			// if statements first
			if(!strcmp(directive, "endif"))
			{
				if(_LookingForEndif <= 0)
				{
					fprintf(stderr, "\033[1;31merror\e[0m: #endif without #if\n");
					return -1;
				}
				_IfEncountered = false;
				_ElseEncountered = false;

				_LookingForEndif--;
				DEBUG_PRINT("endif token!\n");

				_EndifNum++;

//				ifStack->addNode(ifStack, name, value, node);
				
				isSkip = false;
			}
			else if(!strcmp(directive, "ifndef") || !strcmp(directive,"ifdef") || !strcmp(directive, "if") )
			{
				_LookingForEndif++;
				_IfEncountered = true;
				_ElseEncountered = false;
				_IfNum++;
//				ifStack->addNode(ifStack, name, value, node);
				
				if(!strcmp(directive, "if"))
				{
					// evaluate the if statement ...
//					printf("tokenlist of if-group start:\n");

					// remove '#' and 'if' so only the statement gets forwarded for evaluation
					tokenList.removeNode(&tokenList, tokenList.head);
					tokenList.removeNode(&tokenList, tokenList.head);
					evaluateExpression(&tokenList, syntaxTree, symbolTable);
				}
				
				// if true
				isSkip = true;
				
				// evaluate if statement
			}
			else if(!strcmp(directive, "define"))
			{
				DEBUG_PRINT("Define: %s\n", name ? name : "null");
				// if symbol already exists, overwrite it with the new value
				_Symbol * symbol = symbolTable->find(symbolTable, name);
				if(symbol)
				{
					fprintf(stderr, "\033[95mwarning:\033[0m \"%s\" redefined\n  %d | %s\n    |\n", symbol->name, symbol->filePosition->index, symbol->filePosition->data);
					if(symbol->value)
					{
						free(symbol->value);
					}
					// default implicit definition of macro to 1 / true if no value is given
					if(!value)
					{
						value = "1";
					}
					DEBUG_PRINT("tokenValue: %s %d\n", value, (int)strlen(value));
					symbol->value = malloc(sizeof(char) * strlen(value)+1);
					symbol->value[0] = '\0';
					strncpy(symbol->value, value, strlen(value));
					symbol->value[strlen(value)] = '\0';
					DEBUG_PRINT("Re-Define token %s %d!\n", value, (int)strlen(value));
				}
				else
				{
					DEBUG_PRINT("Define token %s @ %d : %d!\n", name, node->index, value ? (int)strlen(value) : -1);
					symbolTable->addNode(symbolTable, name, value, node);							
				}
				
			}
			else if(!strcmp(directive, "pragma"))
			{
				DEBUG_PRINT("pragma token!\n");
			}
			// more token definitions
			else if(!strcmp(directive, "undef"))
			{
				_Symbol * symbol = symbolTable->find(symbolTable, name);
				if(symbol)
				{
					symbolTable->removeNode(symbolTable, symbol);
				}
				DEBUG_PRINT("undef token!\n");
			}
			// conditional
			else if(!strcmp(directive, "elif"))
			{
				if(!_IfEncountered || (_LookingForEndif <= 0))
				{
					fprintf(stderr, "\033[1;31merror\e[0m: #elif without #if\n");
					return -1;
				}
//				ifStack->addNode(ifStack, name, value, node);
			}
			else if(!strcmp(directive, "else"))
			{
				if( (!_IfEncountered) || (_LookingForEndif <= 0) || (_ElseEncountered) )
				{
					fprintf(stderr, "\033[1;31merror\e[0m: #else without #if or #elif\n");
					return -1;
				}
				_ElseEncountered = true;
//				ifStack->addNode(ifStack, name, value, node);
			}
			// special
			else if(!strcmp(directive, "include"))
			{
				// expand include
//				printf("Including file:\n");
//				handleIncludeDirective(&tokenList);
			}
			else if(!strcmp(directive, "line"))
			{
				// set __LINE__ macro
			}
			else if(!strcmp(directive, "error"))
			{
				// throw custom compiler error when reaching this
			}
			// Could not match any of the valid preprocessor directives -- Error!
			else
			{
				fprintf(stderr, "\033[1;31merror:\e[0m invalid preprocessing directive: %s;\n", line);
				return -1;
			}
			
			// add all the preprocessor directives to a list
//			directiveList.addNode(&directiveList, name, value ? (line+strlen(name)+2) : value, node);
		}
		node = node->next;
	}
	
	DEBUG_PRINT("Number of #if statements: %d\n", _IfNum);
	DEBUG_PRINT("Number of #endif statements: %d\n", _EndifNum);

	// confirm that number of ifgroups is same as number of endifs
	if(_IfNum != _EndifNum)
	{
		if(_IfNum > _EndifNum)
		{
//			fprintf(stderr, "\033[1;31merror\e[0m: unterminated %s\n", ifStack->tail->name);
			fprintf(stderr, "\033[1;31merror\e[0m: unterminated ...\n");
			return -1;
		}
		else
		{
			fprintf(stderr, "\033[1;31merror\e[0m: #endif without #if\n");
			return -1;
		}
	}
	
//	printf("\nSymbol table:\n");
//	symbolTable->print(symbolTable);
	
//	DEBUG_PRINT("\nDirective list:\n");
//	directiveList.print(&directiveList);
//	handleIfGroups(&directiveList, fileList, symbolTable, directiveList.head);

//	DEBUG_PRINT("\nifstack print:\n");
//	ifStack->print(ifStack);
		
	return 0;
}

int populateSymbolTable(_LinkedStringList * list)
{
	if(!list)
	{
		DEBUG_PRINT("_LinkedStringList * was NULL\n");
		return -1;
	}
	if(!list->head)
	{
		DEBUG_PRINT("_LinkedStringList head was NULL\n");
		return -1;		
	}
	
	_StringNode * node = list->head;
	while(node)
	{
		char * line = node->data;
		
		// sanitize include statement
		char * preprocessorDirective = "#";
		char * include = strstr(line, preprocessorDirective);
		int includePos = abs(line - include);
		if(include && !partOfString(line, include))
		{
			if(includePos > 1)
			{
				DEBUG_PRINT("includePos: %d\n", includePos);
				while(isWhiteSpace(*line)) line++; // trim whitespaces
				if(line != include)
				{
					DEBUG_PRINT("Oh no, there was a non-whitespace character before the #include!\n");
					return -1;
				}
			}
			int len = strlen(line) - (includePos + strlen(preprocessorDirective));
			char fileName[len]; fileName[0] = '\0';
			strncat(fileName, (line + strlen(preprocessorDirective)), len);
			trimWhiteSpaces(fileName);
			
			// now its possible to open the file and preprocess that too
			int isValid = validateInclude(fileName);
			if(isValid)
			{
				FILE * FP = fopen(fileName, "r");
				if(!FP)
				{
					DEBUG_PRINT("Could not open include file %s\n", fileName);
					return -1;
				}
				
				
			}

			printf("isValid: %d\n", isValid);
		}
		node = node->next;
	}	
	return 0;	
}

int 
expandIncludes(_LinkedStringList * list)
{
	if(!list)
	{
		DEBUG_PRINT("_LinkedStringList * was NULL\n");
		return -1;
	}
	if(!list->head)
	{
		DEBUG_PRINT("_LinkedStringList head was NULL\n");
		return -1;		
	}
	
	_StringNode * node = list->head;
	while(node)
	{
		char * line = node->data;
		
		// sanitize include statement
		char * includeGuard = "#include ";
		char * include = strstr(line, includeGuard);
		int includePos = abs(line - include);
		if(include && !partOfString(line, include))
		{
			if(includePos > 1)
			{
				DEBUG_PRINT("includePos: %d\n", includePos);
				while(isWhiteSpace(*line)) line++; // trim whitespaces
				if(line != include)
				{
					DEBUG_PRINT("Oh no, there was a non-whitespace character before the #include!\n");
					return -1;
				}
			}
			int len = strlen(line) - (includePos + strlen(includeGuard));
			char fileName[len]; fileName[0] = '\0';
			strncat(fileName, (line + strlen(includeGuard)), len);
			trimWhiteSpaces(fileName);
			
			// now its possible to open the file and preprocess that too
			int isValid = validateInclude(fileName);
			if(isValid)
			{
				FILE * FP = fopen(fileName, "r");
				if(!FP)
				{
					DEBUG_PRINT("Could not open include file %s\n", fileName);
					return -1;
				}
				
				
			}

			printf("isValid: %d\n", isValid);
		}
		node = node->next;
	}	
	return 0;
}

int addSyntaxTreeNode(_SyntaxTree * tree, char * str);
int searchSyntaxTreeNode(_SyntaxTree * tree, char * string);

struct _SyntaxTreeNode
{
	int isLeaf;
	int depth;
	int size;
	char data;
	_SyntaxTreeNode ** children;
};

struct _SyntaxTree
{
	int size;
	_SyntaxTreeNode ** children;

	int(*add)(_SyntaxTree * tree, char * string);
	int(*search)(_SyntaxTree * tree, char * string);
};

int _initSyntaxTreeNode(_SyntaxTreeNode * node, bool isLeaf, int depth, int size, char data)
{
//	node = malloc(sizeof(_SyntaxTreeNode));
	
	node->isLeaf = isLeaf;
	node->depth = depth;
	node->size = size;
	node->data = data;
	node->children = NULL;
	
	DEBUG_PRINT("_InitSyntaxTreeNode '%c' isLeaf=%s\n", data, isLeaf ? "true" : "false");
	
	return 0;
}

int searchSyntaxTreeNode(_SyntaxTree * tree, char * string)
{
	// error checking
	if(!tree) 	{ DEBUG_PRINT("_SyntaxTree * tree was NULL\n"); 	return -1; }
	if(!string) { DEBUG_PRINT("char * str was NULL\n"); 			return -1; }

	int length = strlen(string);
	if(!length)
	{
		DEBUG_PRINT("char * str was empty!\n"); return -1;
	}

	DEBUG_PRINT("searched string: %s\n", string);

	int index = 0;
	_SyntaxTreeNode *** list = &tree->children;

	int listSize = tree->size;
	bool isFound = *list == NULL ? false : true;
	bool isLeaf = false;
	int returnIndex = 0;
	while(isFound && (index < length))
	{
		isFound = false;
		DEBUG_PRINT("\t\tTrying to match letter '%c' at index %d with node ...\n\t\t\tLetters: ", string[index], index);		
		if(*list)
		{
			for(int child_i = 0; child_i < listSize; child_i++)
			{
				DEBUG_PRINT("'%c', ", (*list)[child_i]->data);
				if((*list)[child_i]->data == string[index])
				{
					index += 1;
					
					DEBUG_PRINT("... \n\t\tMatched letter '%c' with node %d:%d! \n", string[index], index, child_i);			
					_SyntaxTreeNode * node = (*list)[child_i];
					isLeaf = node->isLeaf;
					list = &node->children;
					listSize = node->size;
					isFound = true;
					
					if(isLeaf)
					{
						returnIndex = index;
					}
					
					break;
				}
			}
		}
		if(!isFound)
		{
			DEBUG_PRINT("\n\t\tCould not find a match!\n");
		}
	}
	
//	if(returnIndex) printf("ReturnIndex: %d\n", returnIndex);
	
	return returnIndex;
}

int addSyntaxTreeNode(_SyntaxTree * tree, char * string)
{
	DEBUG_PRINT("int addSyntaxTreeNode(_SyntaxTree * = '%s', char * str = '%s')\n", "tree", string);
	DEBUG_PRINT("==============================================================================================\n");
	
	// error checking
	if(!tree) 	{ DEBUG_PRINT("_SyntaxTree * tree was NULL\n"); 	return -1; }
	if(!string) { DEBUG_PRINT("char * str was NULL\n"); 			return -1; }

	int index = 0;
	int length = strlen(string);

	_SyntaxTreeNode * node = NULL;
	_SyntaxTreeNode *** list = &tree->children;

	// if the tree is empty, create the first node
	DEBUG_PRINT("Checking to see if tree is empty ...\n");
	if(!*list)
	{
		DEBUG_PRINT("\tTree was empty -- adding '%c' and advancing ...\n", string[index]);

		(*list) = malloc(sizeof(_SyntaxTreeNode*));
		(*list)[0] = malloc(sizeof(_SyntaxTreeNode));

		_initSyntaxTreeNode(((*list)[0]), index>=(length-1), 1, 0, string[index]);

		node = (*list)[0];
		list = &node->children;

		tree->size = 1;

		index++;
	}
	else
	{
		DEBUG_PRINT("\tTree was was not empty\n");
	}

	int listSize = tree->size;
	bool isFound = *list == NULL ? false : true;
	DEBUG_PRINT("Checking to see if there are letters to match against ...\n");
	if(!isFound)
	{
		DEBUG_PRINT("\tTree child had no children -- skipping letter matching!\n");
	}
	else
	{
		DEBUG_PRINT("\tTree had children -- entering matching loop ...\n");
	}
	while(isFound && (index < length))
	{
		isFound = false;
		DEBUG_PRINT("\t\tTrying to match letter '%c' at index %d with node ...\n\t\t\tLetters: ", string[index], index);		
		for(int child_i = 0; child_i < listSize; child_i++)
		{
			DEBUG_PRINT("'%c', ", *list ? (*list)[child_i]->data : 'X'); //getchar();
			if(*list && (*list)[child_i]->data == string[index])
			{
				DEBUG_PRINT("... \n\t\tMatched letter '%c' with node %d:%d! \n", string[index], index, child_i);			
				node = (*list)[child_i];
				list = &node->children;
				listSize = node->size;
				isFound = true;
				break;
			}
		}
		if(!isFound)
		{
			DEBUG_PRINT("\n\t\tCould not find a match!\n");
		}
		else
		{
			index++;
		}
	}
	DEBUG_PRINT("Checking if node is at beginning of tree or not: %s...\n", node ? "false" : "true");
	
	// check whether we are at the beginning of the tree or not
	DEBUG_PRINT("Was there a complete match with the string? -- %s", isFound ? "true" : "false");
	if(!isFound)
	{
		if(node)
		{
			DEBUG_PRINT("\tnode->size++\n");
			node->size++;
		}
		else
		{
			DEBUG_PRINT("\ttree->size++\n");
			tree->size++;
		}
		listSize++;
	}
	
	// the node is not in the list, so add it!
	DEBUG_PRINT("if((index(=%d) <= length(=%d)) && *list) ... %s\n", index, length, ((index <= length) && *list) ? "true -- entering loop" : "false -- skipping");
	if((index < length) && *list)
	{
		DEBUG_PRINT("\tCould not match letter '%c' with node at depth %d -- making list bigger (%d) and adding letter '%c'... \n", string[index], index, listSize, string[index]);
		(*list) = realloc((*list), sizeof(_SyntaxTreeNode*) * listSize);
		(*list)[listSize-1] = malloc(sizeof(_SyntaxTreeNode));
		_initSyntaxTreeNode((*list)[listSize-1], index>=(index-1), index, 0, string[index]);		
		list = &(*list)[listSize-1]->children;
		index++;
	}
	
	// There are more letters but no more existing lists, make more!
	DEBUG_PRINT("while(index(=%d) < length(=%d)) ... %s\n", index, length, (index < length) ? "true -- entering loop": "false -- skipping");
	while(index < length)
	{
		DEBUG_PRINT("\t-- Next list was empty ; allocating new list and node at depth %d with letter '%c' -- advancing ...\n", index, string[index]);

		(*list) = malloc(sizeof(_SyntaxTreeNode*));
		(*list)[0] = malloc(sizeof(_SyntaxTreeNode));
		_initSyntaxTreeNode(((*list)[0]), index>=(length-1), 1, 0, string[index]);
		(*list)[0]->size = 1;
		list = &(*list)[0]->children;
		
		index++;
	}
	
	DEBUG_PRINT("Tree child nodes: %d\n\t", tree->size);
	for(int i = 0; i < tree->size; i++)
	{
		DEBUG_PRINT("'%c' ", tree->children[i]->data);
	}
	DEBUG_PRINT("\n==============================================================================================\n");

	return 0;
}

_SyntaxTree syntaxTree_PreprocessorDirectives =
{
	.size = 0,
	.children = NULL,
	.add = addSyntaxTreeNode,
	.search = searchSyntaxTreeNode
};

char * _Punctuation[] = 
{
	".", ",", ";", ":",
	"(", ")", "{", "}",
	"[", "]", "'", "\"",
	// operators
	"!", "+", "-", "=",
	"<", ">", "*", "/",
	"&", "|", "%", "^",
	"~", "#", "?",
	// double operators
	"--", "++", "||", "&&", "<<", ">>", "##", "->",
	"-=", "+=", "|=", "&=", "<=", ">=", "==", "!=",
	"^=", "*=", "/=", "%=", "<<=", ">>="
};

char * _PreprocessorIdentifiers[] =
{
	"include", "ifndef", "pragma", "define", "endif", 
	"ifdef", "undef", "error", "elif", "else", "line", 
	"if", "__LINE__", "__FILE__"
};

int
processToken(char * string, _LinkedStringList * tokenList, _SyntaxTree * syntaxTree)
{
	if(!string)
	{
		DEBUG_PRINT("matchToken() -- char * string is NULL!");
		return -1;
	}
	if(!strlen(string))
	{
		DEBUG_PRINT("matchToken() -- char * string is empty!");
		return -1;
	}
	
	trimWhiteSpaces(string);
	if(!strlen(string))
	{
		DEBUG_PRINT("Trimmed string into nothingness!\n");
		return -1;
	}
	
	char * ogString = string;
	
	int index = 0;
	
	DEBUG_PRINT("========================\n");
	DEBUG_PRINT("string: '%s'\n", string);
	DEBUG_PRINT("strlen: %d\n", (int)strlen(string));
	char token[strlen(string)+1];
	token[0] = '\0';
	
	int test=7;
	
	bool isNumber = false;
	
	DEBUG_PRINT("> ");
	
	int i = 0;
	while(*string)
	{
		// seperate tokens based on whitespace
		if((i = syntaxTree->search(syntaxTree, string)) > 0)
		{
			if(index > 0)
			{
				token[index] = '\0';
				if(isNumber)
				{
					DEBUG_PRINT("NUMBER: %s\n", token);
					tokenList->addNode(tokenList, token, NumberToken);
					isNumber = false;
				}
				else
				{
					DEBUG_PRINT("IDENT: %s\n", token);
					tokenList->addNode(tokenList, token, IdentifierToken);
				}
			}
//			i+=1;
			token[0] = '\0';
			strncpy(token, string, i);
			token[i] = '\0';

			DEBUG_PRINT("PUNC: %s\n", token);
			tokenList->addNode(tokenList, token, PunctuationToken);
			token[0] = '\0';
			
			string += i;
			index = 0;
		}
		else if(*string == ' ')
		{
			if(index > 0)
			{
				token[index] = '\0';
				if(isNumber)
				{
					DEBUG_PRINT("NUMBER: %s (%s);\n", token, ogString);
					tokenList->addNode(tokenList, token, NumberToken);
					isNumber = false;
				}
				else
				{
					DEBUG_PRINT("IDENT: %s;\n", token);
					tokenList->addNode(tokenList, token, IdentifierToken);
				}

				token[0] = '\0';
				index = 0;
			}
			string++;
			continue;
		}
		else
		{
			// asign letter to token string
			int num = (*string) - '0';
			
//			printf("num: %c %d\n", *string, num);
			
			token[index] = *string++;

			if(index == 0)
			{
				if((num >= 0) && (num <= 9))
				{
					DEBUG_PRINT("num: %c : full_line=%s\n", token[0], ogString);
					isNumber = true;
				}
			}
			
			index++;
			
			if((*string == '\0') && strlen(token))
			{
				token[index] = '\0';
				if(isNumber)
				{
					DEBUG_PRINT("NUMBER: %s\n", token);
					tokenList->addNode(tokenList, token, NumberToken);
					isNumber = false;
				}
				else
				{
					DEBUG_PRINT("IDENT: %s\n", token);
					tokenList->addNode(tokenList, token, IdentifierToken);
				}
				index = 0;
				token[index] = '\0';
			}
		}
	}
	DEBUG_PRINT("\n");
		
	return 0;
}

int
lineTokenization(char * string, _LinkedStringList * tokenList, _SyntaxTree * syntaxTree)
{
	int length = strlen(string);
	char token[length+1];
	token[0] = '\0';

	int index = 0;
	bool isStringLiteral = false;
		
	// clean the string ...
	while(isWhiteSpace(*string)) string++;
	trimWhiteSpaces(string);
		
	DEBUG_PRINT("> %s\n", string);
	for(int i = 0; i < length; i++)
	{
		token[index] = string[i];
		index++;
		
		if(!isStringLiteral)
		{
			if((i==length && strlen(token)) || (isWhiteSpace(string[i]) && (strlen(token) > 0)) )
			{
				// terminate token string
				token[index] = '\0';
				if(strlen(token))
				{
					processToken(token, tokenList, syntaxTree);
				}
				index = 0;
				token[0] = '\0';
			}
		}
		
		if(string[i] == '\"')
		{
			// make sure it is not escaped
			if( (i == 0) || ((string[i-1] != '\\') || ((i>1) && (string[i-2] == '\\')) ) )
			{
				// closing bracket -- tokenize
				if(isStringLiteral)
				{
					isStringLiteral = false;
					token[index] = '\0';
					if(strlen(token))
					{
						printf("> STRING:%s\n", token);
						tokenList->addNode(tokenList, token, StringToken);
					}				
					token[0] = '\0'; 
					index = 0;
				}
				else
				{
					// encountered start of string literal, make everything before it its own token
					if((index > 0) && (i > 0) && strlen(token))
					{								
						index--;
						token[index] = '\0';
						if(strlen(token))
						{
							processToken(token, tokenList, syntaxTree);
						}
						// make beginning of string literal
						token[0] = string[i];
						token[1] = '\0';
						index = 1;
					}
					isStringLiteral = true;
				}
			} // check whether character is escaped
		} // check for start/end ' " ' of string literal
	} // for
	
	return 0;
}

int
tokenization(_LinkedStringList * list, _LinkedStringList * tokenList, _SyntaxTree * syntaxTree)
{	
	_StringNode * node = list->head;
	
	int tokenNum = 0;
	while(node)
	{
		char * string = node->data;
		int length = strlen(string);
		char token[length+1];
		token[0] = '\0';

		// if its a preprocessor directive ...

		int index = 0;
		
		bool isStringLiteral = false;
		
		// clean the string ...
		while(isWhiteSpace(*string)) string++;
		trimWhiteSpaces(string);
		
		// if this is a preprocessor directive ...
		if(string && strlen(string) && string[0] == '#')
		{
			// just add the whole thing as is for later unpacking ...
//			printf("%s -- index: %d\n", string, node->index);
			tokenList->addNode(tokenList, string, PreprocessorToken);
			tokenNum++;
		}
		else
		{
			DEBUG_PRINT("> %s\n", string);
			for(int i = 0; i < length; i++)
			{
				token[index] = string[i];
				index++;
				
				if(!isStringLiteral)
				{
//					printf("(i(%d)==(length-1)(%d) && strlen(token)(%d)) == %d\n", i, length, strlen(token), ((i==(length-1)) && strlen(token)));
//					printf("(isWhiteSpace(string[i(%d)])(%d) && strlen(token)(%d)) == %d\n", i, isWhiteSpace(string[i]), strlen(token), (isWhiteSpace(string[i]) && strlen(token)));
					if(((i==(length-1)) && strlen(token)) || (isWhiteSpace(string[i]) && strlen(token)) )
					{
						// terminate token string
						token[index] = '\0';
//						printf("token: %s\n", token);
						if(strlen(token))
						{
							processToken(token, tokenList, syntaxTree);
						}
						index = 0;
						token[0] = '\0';
					}
				}
				
				if(string[i] == '\"')
				{
					// make sure it is not escaped
					if( (i == 0) || ((string[i-1] != '\\') || ((i>1) && (string[i-2] == '\\')) ) )
					{
						// closing bracket -- tokenize
						if(isStringLiteral)
						{
							isStringLiteral = false;
							token[index] = '\0';
							if(strlen(token))
							{
								DEBUG_PRINT("> STRING:%s\n", token);
								tokenList->addNode(tokenList, token, StringToken);
							}				
							token[0] = '\0'; 
							index = 0;
						}
						else
						{
							// encountered start of string literal, make everything before it its own token
							if((index > 0) && (i > 0) && strlen(token))
							{								
								index--;
								token[index] = '\0';
								if(strlen(token))
								{
									processToken(token, tokenList, syntaxTree);
								}
								// make beginning of string literal
								token[0] = string[i];
								token[1] = '\0';
								index = 1;
							}
							isStringLiteral = true;
						}
					} // check whether character is escaped
				} // check for start/end ' " ' of string literal
			} // for
		} // else -- not a # directive
		node = node->next;
	} // for each char in char *

	DEBUG_PRINT("tokenization directive num: %d\n", tokenNum);

	return 0;
}

int
preprocess(const char * filePATH, const char * outPATH)
{
	_SyntaxTree * syntaxTree = malloc(sizeof(_SyntaxTree));
	syntaxTree->size = 0;
	syntaxTree->children = NULL;
	syntaxTree->add = addSyntaxTreeNode;
	syntaxTree->search = searchSyntaxTreeNode;
	
	_LinkedStringList * _FileLines = malloc(sizeof(_LinkedStringList));
	initStringList(_FileLines);
	
	_Symbol_Table * _SymbolTable = malloc(sizeof(_Symbol_Table));
	initSymbolTable(_SymbolTable);

	readFromFile(filePATH, _FileLines);
	stitchTogether(_FileLines);
	filterOutComments(_FileLines);

	int len = sizeof(_Punctuation) / sizeof(char) / sizeof(char*); 
	printf("Punctuation:\n");
	for(int i = 0; i < len; i++) 
	{
		printf("%s ", _Punctuation[i]);
		addSyntaxTreeNode(syntaxTree, _Punctuation[i]);
	}
	printf("\n");
	
	printf("Numbers: %d - %d\n", '0' - '0', '9' - '0');
	printf("\n");
	
	len = sizeof(_PreprocessorIdentifiers) / sizeof(char) / sizeof(char*);
	printf("Preprocessor identifiers:\n");
	for(int i = 0; i < len; i++)
	{
		printf("%s ", _PreprocessorIdentifiers[i]);
		syntaxTree_PreprocessorDirectives.add(&syntaxTree_PreprocessorDirectives, _PreprocessorIdentifiers[i]);
	}
	printf("\n");

	_LinkedStringList * _Tokens = malloc(sizeof(_LinkedStringList));
	initStringList(_Tokens);

	// tokenization
	tokenization(_FileLines, _Tokens, syntaxTree);	
	handlePreprocessorDirectives(_FileLines, syntaxTree, _SymbolTable);
//	expandIncludes(&_FileLines);
	
//	_Tokens->print(_Tokens, false);
//	_FileLines->print(_FileLines, false);

	return 0;
}

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

	preprocess("preprocessor.c", "preprocessor.E");

	return 0;
}