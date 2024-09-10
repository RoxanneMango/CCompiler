#include "string_utils.h"

int
initStringList(_LinkedStringList * list)
{
	list->head = NULL;
	list->current = NULL;
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
		list->head = NULL;
		list->current = NULL;
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

int addStringNode(_LinkedStringList * list, const char * string)
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
	int len = strlen(string);
	node->length = len;
	node->data = malloc(sizeof(char) * len+1);
	node->data[0] = '\0';
	node->index = 0;
	strncpy(node->data, string, len);

	node->next = NULL;

	// List has no nodes yet, make this the first node
	if(!list->current)
	{
		node->index = 0;
		node->next = NULL;
		node->previous = NULL;
		
		list->length = 1;
		list->head = node;
		list->current = node;
	}
	else
	{
		node->index = list->length;
		node->previous = list->current;
		node->next = NULL;

		list->current->next = node;
		list->current = node;

		list->length++;
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
	bool newLine = true;

	if(prettyPrint)
	{
		while(node)
		{
			if(newLine)
			{
				printf("%d\t| %s", node->index, node->data);
				newLine = false;
			}
			else
			{
				printf("%s", node->data);
			}
			newLine = strstr(node->data, "\n");
			node = node->next;
		}
	}
	else
	{
		while(node)
		{
			printf("%s\n", node->data);
			node = node->next;
		}
	}

	return 0;
}

int
stitchTogetherFromFile(const char * filePATH, const char * outPATH)
{
	printf("stitchTogether() ...\n");
	
	// parse the file before processing it
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
				DEBUG_PRINT("Unable to reallocate memory for the line buffer\n");
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
			char * splitString = strstr(line, "\\");
			if(splitString)
			{
				int splitPos = abs(line - splitString)+1;
				int stringLen = strlen(line);

//				printf("split string: %d ; %d\n", splitPos, stringLen);

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
						line[splitPos-1] = '\0';
					}
				}
			}
			fputs(line, FP_OUT);
		}
	}
	fclose(FP);
	fclose(FP_OUT);
	free(line);
	
	DEBUG_PRINT("\n\nMax // line size: %zd\n", len);
	
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
					line[splitPos-1] = '\0';
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