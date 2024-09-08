#include <stdlib.h>	// standard memory operations, etc
#include <stdio.h>  // standard IO
#include <string.h>	// string manipulations
#include <stdbool.h>// true false bool definition
#include <dirent.h> // DIR * definition

#include "string_utils.h"
#include "symbol_table.h"

#ifndef bla
#define bla
#if true
#elif true
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
	.current = NULL, 
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
static const int _reservedDirectivesNum = 13;
char * _ReservedDirectives[] =
{
	"#defined", "#include", "#ifndef", "#pragma", "#define", "#endif", "#ifdef", "#undef", "#error", "#elif", "#else", "#line", "#if"
};

const char * LINE_MACRO = "__Line__";
const char * FILE_MACRO = "__FILE__";

enum _KeyWord
{
	_Defined_Token = 0,
	_Include_Token,
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
			_FileLines.addNode(&_FileLines, line);
		}
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
	
	char * previousDirective = NULL;
	int _LookingForEndif = 0;
	int _ElseEncountered = 0;
	
	
	// initialize the #if statement stack to push and pop to
	symbolTable.ifStack = malloc(sizeof(_Symbol_Table));
	initSymbolTable(symbolTable.ifStack);
	_Symbol_Table * ifStack = symbolTable.ifStack;
	
	_StringNode * node = list->head;
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
			bool isValid = false;
			while(isWhiteSpace(*line)) line++; // trim whitespaces
			
			for(int i = 0; i < _reservedDirectivesNum; i++)
			{
				char * ppd = strstr(line, _ReservedDirectives[i]);
				if(ppd)
				{
					char * name = _ReservedDirectives[i];
					char * value = (line+(strlen(_ReservedDirectives[i]))+strlen(poundSign));

					while(isWhiteSpace(*value)) value++;
					trimWhiteSpaces(value);

					// #ifdef is short for: #if defined(X)
					// #ifndef is short for: #if !defined(X)

					// if statements first
					if(name == _ReservedDirectives[_Ifndef_Token])
					{
						_LookingForEndif++;
						
						printf("ifndef token!\n");
						ifStack->addNode(ifStack, name, value);
						
						// look through symbol table to see if symbol has been defined already
						if(!symbolTable.find(&symbolTable, name, value))
						{
							// symbol was not found, execute everything between this and the #endif.
						}
						else
						{
							// remove everything from file between this and the #endif
						}
						// remove the preprocessor directive from the file list
					}
					else if(name == _ReservedDirectives[_Ifdef_Token])
					{
						_LookingForEndif++;
						ifStack->addNode(ifStack, name, value);
						
						printf("ifdef token!\n");
						// look through symbol table to see if symbol has been defined already
						if(!symbolTable.find(&symbolTable, name, value))
						{
							// symbol was not found, execute everything between this and the #endif.
						}
						else
						{
							// remove everything from file between this and the #endif
						}
						// remove the preprocessor directive from the file list
					}
					else if(name == _ReservedDirectives[_If_Token])
					{
						_LookingForEndif++;
						printf("if token!\n");
						
						ifStack->addNode(ifStack, name, value);
						
					}
					else if(name == _ReservedDirectives[_Define_Token])
					{}
					else if(name == _ReservedDirectives[_Endif_Token])
					{
						if(_LookingForEndif <= 0)
						{
							DEBUG_PRINT("error: #endif without #if\n");
							return -1;
						}
						_LookingForEndif--;
						_ElseEncountered = false;
						printf("endif token!\n");

						ifStack->addNode(ifStack, name, value);
					}
					else if(name == _ReservedDirectives[_Pragma_Token])
					{}
					// more token definitions
					else if(name == _ReservedDirectives[_Undef_Token])
					{}
					else if(name == _ReservedDirectives[_Defined_Token])
					{}
					// conditional
					else if(name == _ReservedDirectives[_Elif_Token])
					{
						if(_LookingForEndif <= 0)
						{
							DEBUG_PRINT("error: #elif without #if\n");
							return -1;
						}
					}
					else if(name == _ReservedDirectives[_Else_Token])
					{
						if( (_LookingForEndif) || (_ElseEncountered) )
						{
							DEBUG_PRINT("error: #else without #if\n");
							return -1;
						}
						_ElseEncountered = true;
					}
					else if(name == _ReservedDirectives[_Defined_Token])
					{}
					// special
					else if(name == _ReservedDirectives[_Include_Token])
					{}
					else if(name == _ReservedDirectives[_Line_Token])
					{}
					else if(name == _ReservedDirectives[_Error_Token])
					{}
					
					symbolTable.addNode(&symbolTable, name, value);
					
					isValid = true;

					previousDirective = name;
					break;
				}
			}
		}
		
		node = node->next;
	}
	

	ifStack->print(ifStack);
	
	// one node goes forward, the other backwards;
		
	// if there is an even number of if and endif statements
	if(!(ifStack->length%2))
	{
		printf("ifStack length: %d\n", ifStack->length);

		_Symbol * ifGroup = ifStack->head;
		_Symbol * endif = ifStack->head;
		while((ifGroup != ifStack->tail) && (endif != ifStack->tail))
		{
			printf("\ifGroup\t");
			while(ifGroup)
			{
				if(strcmp(ifGroup->name,_ReservedDirectives[_Endif_Token]))
				{
					printf("%d\t| %s\n", ifGroup->index, ifGroup->name);
					ifGroup = ifGroup->next;
					break;
				}
				ifGroup = ifGroup->next;
			}
			printf("endif\t");
			while(endif)
			{
				if(strcmp(endif->name, _ReservedDirectives[_Endif_Token]) == 0)
				{
					printf("%d\t| %s\n", endif->index, endif->name);
					endif = endif->next;
					break;
				}
				endif = endif->next;
			}
		}
	}
	
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

int
preprocess(const char * filePATH, const char * outPATH)
{
	readFromFile(filePATH);

	stitchTogether(&_FileLines);
	filterOutComments(&_FileLines);
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
