#include "string_utils.h"

int
initStringList(_LinkedStringList * list)
{
	list->head = NULL;
	list->tail = NULL;
	list->addNode = addStringNode;
	list->removeNode = removeStringNode;
	list->print = printStringList;
	
	return 0;
}

int removeStringNode(_LinkedStringList * list, _StringNode * node)
{
	if(!list)
	{
		DEBUG_PRINT("_LinkedStringList * list was NULL\n");
		return -1;
	}
	if(!node)
	{
		DEBUG_PRINT("_StringNode * node was NULL\n");
		return -1;
	}
	
	_StringNode * next = node->next;
	_StringNode * previous = node->previous;

	if(node == list->head)	// its the start node
	{
		if(next)
		{
			list->head = next;
		}
		else
		{
			list->head = NULL;
			list->tail = NULL;			
		}
		free(node->data);
		free(node);
	}
	else if(next)			// its a node in the middle of the list
	{
		previous->next = next;
		next->previous = previous;
		free(node->data);
		free(node);
	}
	else					// its at the end of the list
	{
		previous->next = NULL;
		free(node->data);
		free(node);
	}
	
	return 0;
}

int addStringNode(_LinkedStringList * list, const char * string, TokenType type)
{
	if(!list)
	{
		DEBUG_PRINT("_LinkedStringList * was NULL\n");
		return -1;
	}
	if(!string)
	{
		DEBUG_PRINT("const char * string was NULL\n");
		return -1;
	} 
	
	_StringNode * node = malloc(sizeof(_StringNode));
	if(!node)
	{
		DEBUG_PRINT("Could not add _StringNode\n");
		return -1;
	}
		
	DEBUG_PRINT("addStringNode string: %s\n", string);
	
	int len = strlen(string);
	node->length = len;
	node->data = malloc(sizeof(char) * len+1);
	node->data[0] = '\0';
	node->index = 0;
	node->type = type;
	strncpy(node->data, string, len);
	node->data[len] = '\0';

	node->next = NULL;

	// List has no nodes yet, make this the first node
	if(!list->tail)
	{
		node->index = 0;
		node->next = NULL;
		node->previous = NULL;
		
		list->length = 1;
		list->head = node;
		list->tail = node;
	}
	else
	{
		node->index = list->length;
		node->previous = list->tail;
		node->next = NULL;

		list->tail->next = node;
		list->tail = node;

		list->length++;
	}
	return 0;
}

int printStringListSimple(_LinkedStringList * list)
{
	if(!list) 		{ DEBUG_PRINT("_LinkedStringList * was NULL\n");	return -1; }
	if(!list->head)	{ DEBUG_PRINT("_LinkedStringList head was NULL\n");	return -1; }
	
	DEBUG_PRINT("list length: %d\n", list->length);
	
	_StringNode * node = list->head;
	while(node)
	{
		printf("%s\n", node->data);
		node = node->next;
	}
	return 0;
}

int printStringList(_LinkedStringList * list, bool prettyPrint)
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
	
	DEBUG_PRINT("list length: %d\n", list->length);
	
	_StringNode * node = list->head;

//	int lineNumber = 1;

	if(prettyPrint)
	{
		while(node)
		{
			printf("%d\t| %s", node->index, /*lineNumber++,*/ node->data);
			node = node->next;
		}
	}
	else
	{
		bool wasIdent = false;
		int indent = 0;
		while(node)
		{
			if(node->type != IdentifierToken)
			{
				wasIdent = false;
			}
			if(node->type == IdentifierToken)
			{
				printf("%s%s%s%s", "\033[90m", wasIdent ? " " : "", node->data, "\033[0m"); // blue
				wasIdent = true;
			}
			else if(node->type == PunctuationToken)
			{
				char * cc_start = "\033[94m"; // default blue
				char * cc_end = "\033[0m";
				
				char c = node->data[0];
				if(c=='+' || c=='-' || c=='/' || c=='*' || c=='%') // || c=='#' || c=='&')
				{
					node->type = OperatorToken;
					cc_start = "\033[96m"; // cyan
				}
				else if((((strlen(node->data)==1) && c == '<') || c == '>') || strlen(node->data) == 2)
				{
					char c2 = node->data[1];
					if((c2 != '=') || (c == '=' && c2 == '='))
					{
						node->type = ComparisonToken;
						cc_start = "\033[91m"; // red
					}
				}
				else if(((strlen(node->data)==1) && c == '=') || (strlen(node->data)==2 && node->data[1] == '=') )
				{
					// assignment token
					node->type == AssignmentToken;
					cc_start = "\033[95m"; // magenta
				}
				printf("%s%s%s", cc_start, node->data, cc_end);

				if(c=='{') indent += 1;
				if(c=='}') indent -= 1;
				
				if(c == ';' || c == '{' || c == '}')
				{
					printf("\n");
					if(indent)
					{
						for(int i = 0; i < indent; i++)
						{
							printf("  ");
						}						
					}
				}
			}
			else if(node->type == NumberToken)
			{
				printf("\033[92m%s\033[0m", node->data); // green
			}
			else if(node->type == StringToken)
			{
				printf("\033[33m%s\033[0m", node->data); // Dark Yellow
			}
			
//			printf("%s\n", node->data);
			node = node->next;
		}
		printf("\n");
	}

	return 0;
}

int
stitchTogether(_LinkedStringList * list)
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
		char * splitString = strstr(line, "\\");

		if(splitString)
		{
			int splitPos = abs(line - splitString)+1;
			int stringLen = strlen(line);

			DEBUG_PRINT("split string: %d ; %d\n", splitPos, stringLen);

			// check if its valid syntax for string splitting
			if( (splitPos < stringLen) &&
				((line[splitPos] == ' ') || 
				(line[splitPos] == '\t') || 
				(line[splitPos] == '\n')) )
			{
				DEBUG_PRINT("split string: %s", line);

				// check for whitespaces
				bool whiteSpaceWarning = (line[splitPos] != '\n');
				bool isValidStitch = true;
				DEBUG_PRINT("whiteSpaceWarning: %d\n\n", whiteSpaceWarning);

				for(int i = splitPos; isValidStitch && (i < stringLen); i++)
				{
					if( (line[i] != ' ') && (line[i] != '\t') && (line[i] != '\n') )
					{
						DEBUG_PRINT("is not Whitespace! : %c\n", line[i]);
						isValidStitch = false;
					}
				}
				if(isValidStitch)
				{
					if(whiteSpaceWarning)
					{
						fprintf(stderr, "warning: backslash and newline separated by space\n");
					}
					// concatinate strings together
					DEBUG_PRINT("Stitching lines together\n");					
					line[splitPos-1] = '\0';
					
					_StringNode * nextNode = node->next;
					
					if(nextNode == NULL)
					{
						fprintf(stderr, "\033[1;31merror\e[0m: stray '\' in program\n");
					}
					char * nextLine = nextNode->data;
					int length1 = strlen(line);
					int length2 = strlen(nextLine);
					char * concat = malloc(sizeof(char) * (length1 + length2) + 1);
					concat[0] = '\0';
					strcat(concat, line);
					strcat(concat, nextLine);
					
					DEBUG_PRINT("(%d + %d) | concat: %s + %s = %s\n", node->index, nextNode->index, line, nextLine, concat);

					// swapperoo the data with our newly allocated string
					list->removeNode(list, nextNode);					
					free(node->data);
					node->data = concat;
					continue;
				}
			}
		}		
		node = node->next;
	}
	return 0;
}

bool 
partOfString(char * line, char * substr)
{
	if(!substr || !line)
	{
		DEBUG_PRINT("partOfString(char *, char *) argument(s) was NULL!");
		return false;
	}
	
	// check if comment is part of a string ...
	char * dq = strstr(line, "\"");
	char * sq = strstr(line, "\'");

	// get starting position of comment
	int a = abs(line - substr);
	
	// There was a quotation mark before comment, is it part of a string?
	if(dq)
	{
		if((abs(line - dq)) < a)
		{
			char * dq2 = strstr(substr, "\"");
			if(dq2 && (*(dq2-1) != '\\'))
			{
				// its part of a string!
//				fputs(line, FP_OUT);
				return true;
			}
		}
	}
	else if(sq && ((abs(line - sq)) < a))
	{
		char * sq2 = strstr(substr, "\"");
		if(sq2 && (*(sq2-1) != '\\'))
		{
			// its part of a string!
			return true;
		}
	}
	return false;
}

bool 
isWhiteSpace(const char c)
{
	return ( (c == ' ') ||  (c == '\t') || (c == '\n') || (c == '\r') );
}

bool 
isOnlyWhiteSpaces(char * line)
{
	if(line)
	{
		for(int i = 0; i < strlen(line); i++)
		{
			if(!isWhiteSpace(line[i]))
			{
				return false;
			}
		}
		return true;
	}
	return false;
}

int
trimWhiteSpaces(char * string)
{
	if(!string)
	{
		DEBUG_PRINT("Could not trim whitespaces from NULL");
		return -1;
	}
	string += (strlen(string)-1);	
	
	if(isWhiteSpace(*string))
	{
		*(string) = '\0';
		trimWhiteSpaces(string);
	}

	return 0;
}