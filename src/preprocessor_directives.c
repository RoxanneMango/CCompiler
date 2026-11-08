#include "preprocessor_directives.h"
#include "string_utils.h"
#include "file_io.h"
#include "tokenization.h"
#include "evaluate_expressions.h"

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
					int result = evaluateExpressions(&tokenList, syntaxTree, symbolTable);
					printf("condition result: %s\n", result==1 ? "true" : "false");
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
