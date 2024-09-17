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

#if defined bla
#undef bla
#endif

#ifdef bla
#undef bla
#endif

// Definition of macros is checked before include expansion (include guards)
// include macros are expanded before defines!

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
_LinkedStringList _FileLines = 
{ 
	.head = NULL, 
	.tail = NULL, 
	.addNode = addStringNode, 
	.removeNode = removeStringNode,
	.print = printStringList
};

_LinkedStringList _Tokens =
{
	.head = NULL,
	.tail = NULL,
	.addNode = addStringNode,
	.removeNode = removeStringNode,
	.print = printStringList
};

_Symbol_Table symbolTable = 
{
	.head = NULL,
	.tail = NULL,
	.addNode = addSymbolNode,
	.removeNode = removeSymbolNode,
	.print = printSymbolTable,
	.find = findSymbolNode
};

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

// define the reserved keywords, sorted in ascending sizes
static const int _reservedDirectivesNum = 12;
char * _ReservedDirectives[] =
{
	"#include", "#ifndef", "#pragma", "#define", "#endif", "#ifdef", "#undef", "#error", "#elif", "#else", "#line", "#if"
};

char * _ReservedOperators[] = 
{
	"+", "-", "*", "/", "%", "=", "==", "!", "!=", "|", "||", "&", "&&", "~", "^"
};

const char * LINE_MACRO = "__LINE__";
const char * FILE_MACRO = "__FILE__";

enum _KeyWord
{
	_Include_Token = 0,
	_Ifndef_Token,
	_Pragma_Token,
	_Define_Token,
	_Endif_Token,
	_Ifdef_Token,
	_Undef_Token,
	_Error_Token,
	_Elif_Token,
	_Else_Token,
	_Line_Token,
	_If_Token
};

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
readFromFile(const char * filePATH)
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
			_FileLines.addNode(&_FileLines, line);
			// empty the line buffer
			line[0] = '\0';
		}
	}
	
	// add the last line
	if(line && strlen(line))
	{
		_FileLines.addNode(&_FileLines, line);
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
			list->addNode(list, dir->d_name);
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
removeBetweenSymbols(_Symbol_Table * list, _Symbol * begin)
{
	_Symbol * end = begin->next;
	int nesting = 0;
	while(end)
	{
		if(	!strcmp(end->name, _ReservedDirectives[_If_Token]) ||
			!strcmp(end->name, _ReservedDirectives[_Ifdef_Token]) ||
			!strcmp(end->name, _ReservedDirectives[_Ifndef_Token]) )
		{
			nesting++;
		}
		if(strcmp(end->name, _ReservedDirectives[_Endif_Token]) == 0)
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
		_FileLines.removeNode(&_FileLines, line);
		line = line->next;
		if(line == b)
		{
			DEBUG_PRINT("\tlast line: %s\n", line->data);
			_FileLines.removeNode(&_FileLines, b);
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
handleIfGroups(_Symbol_Table * list, _Symbol * directive)
{
	int ifndef = false;
	while(directive)
	{
		char * name = directive->name;
		char * value = directive->value;
		
		if((ifndef = (strcmp(name, _ReservedDirectives[_Ifndef_Token]) == 0)) || (strcmp(name, _ReservedDirectives[_Ifdef_Token]) == 0))
		{
			DEBUG_PRINT("%s token!\n", ifndef ? "#ifndef" : "#ifdef");						

			// look through symbol table to see if symbol has been defined already
			_Symbol * foundSymbol = symbolTable.find(&symbolTable, value);
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
				removeBetweenSymbols(list, directive);
				handleIfGroups(list, list->head);
			}
		}
		else if(strcmp(name, _ReservedDirectives[_If_Token]) == 0)
		{																		
			// handle if statement here ...
			DEBUG_PRINT("If token! : %s: %d\n", value, EVAL(value));				
		}
		
		directive = directive->next;
	}
}

int 
handlePreprocessorDirectives(_LinkedStringList * list)
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
	
	int _LookingForEndif = 0;
	int _ElseEncountered = 0;
	int _IfEncountered = 0;
	
	int _IfNum = 0;
	int _EndifNum = 0;
	
	// initialize the #if statement stack to push and pop to
	symbolTable.ifStack = malloc(sizeof(_Symbol_Table));
	initSymbolTable(symbolTable.ifStack);
	_Symbol_Table * ifStack = symbolTable.ifStack;
	
	_StringNode * node = list->head;
	
	_Symbol_Table directiveList;
	initSymbolTable(&directiveList);
	
	// static if statement analysis and adding preprocessor directives to one list for
	// further processing later
	while(node)
	{
		char * line = node->data;

		char * poundSign = "#";
		char * directive = strstr(line, poundSign);
		int directivePos = abs(line - directive);
	
		// confirm there is a preprocessor directive on this line
		if(directive && !partOfString(line, directive))
		{
			// trim whitespaces if there are any
			if(directivePos > 1)
			{
				DEBUG_PRINT("directivePos: %d\n", directivePos);
				while(isWhiteSpace(*line)) line++; // trim whitespaces
				if(line != poundSign)
				{
					DEBUG_PRINT("Oh no, there was a non-whitespace character before the #include!\n");
					return -1;
				}
			}
			// parse the preprocessor directive line
			while(isWhiteSpace(*line)) line++; // trim whitespaces

			bool isValid = false;
			for(int i = 0; (i < _reservedDirectivesNum); i++)
			{
				char * ppd = strstr(line, _ReservedDirectives[i]);
				if(ppd)
				{
					isValid = true;
					
					char * name = _ReservedDirectives[i];
					char * value = (line+(strlen(_ReservedDirectives[i]))+strlen(poundSign));

					while(isWhiteSpace(*value)) value++;
					trimWhiteSpaces(value);

					// #ifdef is short for: #if defined(X)
					// #ifndef is short for: #if !defined(X)

					// if statements first
					if( (name == _ReservedDirectives[_Ifndef_Token]) || (name == _ReservedDirectives[_Ifdef_Token]) || (name == _ReservedDirectives[_If_Token]) )
					{
//						handleIfGroup(list, node, name, value);
						_LookingForEndif++;
						_IfEncountered = true;
						_ElseEncountered = false;
						_IfNum++;
						ifStack->addNode(ifStack, name, value, node);
					}
					else if(name == _ReservedDirectives[_Endif_Token])
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

						ifStack->addNode(ifStack, name, value, node);
					}
					else if(name == _ReservedDirectives[_Define_Token])
					{
						while(isWhiteSpace(*ppd)) ppd++;
						trimWhiteSpaces(ppd);
						DEBUG_PRINT("ppd: %s\n", ppd);
						
						// array has 3 indexes
						char ** dict = malloc(sizeof(char *) * 3);
						int dictLength = 0;
						int wordLength = 0;
						int wordOffset = 0;
						int stringLength = strlen(ppd);
						for(int i = 0; i <= stringLength && dictLength < 3; i++)
						{
							if((ppd[i] == ' ') || (i == stringLength))
							{							
								wordOffset += wordOffset > 0; // skip past the whitespace
								wordLength = i - wordOffset;
								// end of word
								dict[dictLength] = malloc(sizeof(char) * (wordLength+1));
								dict[dictLength][0] = '\0';
								strncpy(dict[dictLength], (ppd+wordOffset), wordLength);
								dict[dictLength][wordLength] = '\0';

								DEBUG_PRINT("word: %s (%d)\n", dict[dictLength], wordLength);

								wordOffset += wordLength;

								dictLength++;
							}
						}

						char * tokenName = dict[1];
						char * tokenValue = dictLength == 3 ? dict[2] : NULL;
						
						DEBUG_PRINT("tokenValue: %s\n", tokenValue ? "DEFINED" : "UNDEFINED");
						
						// if symbol already exists, overwrite it with the new value
						_Symbol * symbol = symbolTable.find(&symbolTable, tokenName);
						if(symbol)
						{
							fprintf(stderr, "\033[95mwarning:\033[0m \"%s\" redefined\n  %d | %s\n    |\n", symbol->name, symbol->filePosition->index, symbol->filePosition->data);
							if(symbol->value)
							{
								free(symbol->value);
							}
							if(tokenValue)
							{
								DEBUG_PRINT("tokenValue: %s %d\n", tokenValue, (int)strlen(tokenValue));
								symbol->value = malloc(sizeof(char) * strlen(tokenValue));
								symbol->value[0] = '\0';
								strcpy(symbol->value, tokenValue);
								symbol->value[strlen(tokenValue)] = '\0';
								DEBUG_PRINT("Re-Define token %s %d!\n", tokenValue, (int)strlen(tokenValue));
							}
						}
						else
						{
							DEBUG_PRINT("Define token %s @ %d : %d!\n", tokenName, node->index, tokenValue ? (int)strlen(tokenValue) : -1);
							symbolTable.addNode(&symbolTable, tokenName, tokenValue, node);							
						}
						
						// free dict after creating symbol
						for(int i = dictLength-1; i >= 0; i--)
						{
							DEBUG_PRINT("FREE: dict[%d] : %s\n", i, dict[i]);
							free(dict[i]);
						}
						free(dict);
						
					}							
					else if(name == _ReservedDirectives[_Pragma_Token])
					{
						DEBUG_PRINT("pragma token!\n");
					}
					// more token definitions
					else if(name == _ReservedDirectives[_Undef_Token])
					{
						DEBUG_PRINT("undef token!\n");
					}
					// conditional
					else if(name == _ReservedDirectives[_Elif_Token])
					{
						if(!_IfEncountered || (_LookingForEndif <= 0))
						{
							fprintf(stderr, "\033[1;31merror\e[0m: #elif without #if\n");
							return -1;
						}
						ifStack->addNode(ifStack, name, value, node);
					}
					else if(name == _ReservedDirectives[_Else_Token])
					{
						if( (!_IfEncountered) || (_LookingForEndif <= 0) || (_ElseEncountered) )
						{
							fprintf(stderr, "\033[1;31merror\e[0m: #else without #if or #elif\n");
							return -1;
						}
						_ElseEncountered = true;
						ifStack->addNode(ifStack, name, value, node);
					}
					// special
					else if(name == _ReservedDirectives[_Include_Token])
					{
						// expand include
					}
					else if(name == _ReservedDirectives[_Line_Token])
					{
						// set __LINE__ macro
					}
					else if(name == _ReservedDirectives[_Error_Token])
					{
						// throw custom compiler error when reaching this
					}
					
					// add all the preprocessor directives to a list
					directiveList.addNode(&directiveList, name, value, node);
					break;
				}
			}
			if(!isValid)
			{
				fprintf(stderr, "\033[1;31merror:\e[0m invalid preprocessing directive: %s\n", line);
				return -1;
			}			
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
			fprintf(stderr, "\033[1;31merror\e[0m: unterminated %s\n", ifStack->tail->name);
			return -1;
		}
		else
		{
			fprintf(stderr, "\033[1;31merror\e[0m: #endif without #if\n");
			return -1;
		}
	}
	
//	printf("\nSymbol table:\n");
//	symbolTable.print(&symbolTable);
	
	DEBUG_PRINT("\nDirective list:\n");
//	directiveList.print(&directiveList);
	handleIfGroups(&directiveList, directiveList.head);

//	printf("\nifstack print:\n");
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

struct _SyntaxTreeNode typedef _SyntaxTreeNode;
struct _SyntaxTree typedef _SyntaxTree;

int addSyntaxTreeNode(_SyntaxTree * tree, char * str);


// make length go up
int searchSyntaxTreeNode(_SyntaxTree * tree, char * string, int length);

int searchSyntaxTreeNode(_SyntaxTree * tree, char * string, int length)
{
	return 0;
}

struct _SyntaxTreeNode
{
	int depth;
	int size;
	char data;
	_SyntaxTreeNode * parent;
	_SyntaxTreeNode * children;
};

struct _SyntaxTree
{
	int size;
	_SyntaxTreeNode * children;

	int(*add)(_SyntaxTree * tree, char * str);
	int(*search)(_SyntaxTree * tree, char * string, int length);
};

int addSyntaxTreeNode(_SyntaxTree * tree, char * str)
{
//	printf("int addSyntaxTreeNode(_SyntaxTree * tree, char * str)\n");
	
	// error checking
	if(!tree) { DEBUG_PRINT("_SyntaxTree * tree was NULL\n"); 	return -1; }
	if(!str)   { DEBUG_PRINT("char * str was NULL\n"); return -1; }
		
	int depth = 0;
	
	int length = strlen(str);
	
	_SyntaxTreeNode * current = tree->children;

	for(int i = 0; current && (i < length); i++)
	{
		bool isFound = false;
		int j = 0;
		while(j < current->size)
		{
			if(str[i] == current[j].data)
			{
				isFound = true;
				current = current[j].children;
				depth++;
				break;
			}
			else
			{
				j++;
			}
		}
		str++;
	}

	for(int i = 0; i < strlen(str); i++)
	{
		if(!current)
		{
			_SyntaxTreeNode * next = malloc(sizeof(_SyntaxTreeNode));
			next->children = NULL;
			next->data = str[i];
			next->size = 1;
			next->depth = i;
			next->parent = current;
			current = next->children;
			
//			printf("%c ", next->data);
		}
	}
	
	return 0;
}

_SyntaxTree syntaxTree = 
{
	.size = 0,
	.add = addSyntaxTreeNode,
	.search = searchSyntaxTreeNode
};

int
tokenization(_LinkedStringList * list)
{
	_StringNode * node = list->head;
	while(node)
	{
		char * string = node->data;
		int length = strlen(string);
		char token[length];
		token[0] = '\0';

		int index = 0;
		
		bool isStringLiteral = false;
		bool isCharLiteral = false;
		
		// clean the string ...
		while(isWhiteSpace(*string)) string++;
		trimWhiteSpaces(string);
	
		for(int i = 0; i <= length; i++)
		{
//			if()


			if( (i==length) || (!isStringLiteral && !isCharLiteral && isWhiteSpace(string[i])) )
			{
				// terminate token string
				token[index] = '\0';

				printf("%s\n", token);
				// Try to find a match with a preprocessing token ...
				if(strlen(token))
					syntaxTree.add(&syntaxTree, token);

				// reset token string
				index = 0;
				token[0] = '\0';
			}
			else
			{
				if(token[index] == '\"')
				{
					isStringLiteral = isStringLiteral ? false : true;
				}
				if(token[index] == '\'')
				{
					isCharLiteral = isCharLiteral ? false : true;
				}
				token[index] = string[i];
				index++;
			}
		
		}
		node = node->next;
	}
	return 0;
}

int
preprocess(const char * filePATH, const char * outPATH)
{
	readFromFile(filePATH);

	stitchTogether(&_FileLines);
	filterOutComments(&_FileLines);
	
	// tokenization
	tokenization(&_FileLines);
	
	handlePreprocessorDirectives(&_FileLines);
//	expandIncludes(&_FileLines);
	
//	_FileLines.print(&_FileLines, true);

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