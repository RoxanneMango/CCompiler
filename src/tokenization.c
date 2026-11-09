#include "tokenization.h"

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
	_SyntaxTreeSearchResult result = {-1, IdentifierToken};
	
	while(*string)
	{
		result = syntaxTree->search(syntaxTree, string);
		i = result.index;
		// seperate tokens based on whitespace
		if(i >= 0)
		{
			// Only a part of the token could be matched against a keyword
			// The other part is an identifier token (IDENT)
			if(index > 0)
			{
				token[index] = '\0';
				tokenList->addNode(tokenList, token, IdentifierToken);
			}
			token[0] = '\0';
			strncpy(token, string, i);
			token[i] = '\0';

			DEBUG_PRINT("tokenType(%d): %s\n", result.type, token);
			tokenList->addNode(tokenList, token, result.type);
			token[0] = '\0';
			
			string += i;
			index = 0;
		}
		else if(*string == ' ')
		{
			if(index > 0)
			{
				token[index] = '\0';
				tokenList->addNode(tokenList, token, result.type);
				token[0] = '\0';
				index = 0;
			}
			string++;
			continue;
		}
		else
		{
			token[index] = *string++;
			index++;
			if((*string == '\0') && strlen(token))
			{
				token[index] = '\0';
				tokenList->addNode(tokenList, token, result.type);
				index = 0;
				token[index] = '\0';
			}
		}
	}
	DEBUG_PRINT("\n");
		
	return 0;
}

int
assignProcessedTokens(_LinkedStringList * tokenList, _SyntaxTree * syntaxTree)
{	
	_StringNode * node = tokenList->head;
	while(node)
	{
		// This token has the default type so we check if it matches against a keyword
		if(node->type == IdentifierToken)
		{
			if((node->data[0] >= '0') && (node->data[0] <= '9')) // number
			{
				node->type = NumberToken;
			}
			// Check if its a keyword
			else
			{
				_SyntaxTreeSearchResult result = syntaxTree->search(syntaxTree, node->data);
				if(result.index >= 0 && result.index == node->length)
				{
					node->type = result.type;
				}				
			}
		}
		node = node->next;
	}
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

	bool isStringLiteral = false;
	bool isEscapedStringLiteral = false;

	while(node)
	{
		char * string = node->data;		
		// clean the string ...
		while(isWhiteSpace(*string)) string++;
		trimWhiteSpaces(string);

		int length = strlen(string);
		char token[length+1];
		token[0] = '\0';

		if(!string || !length) continue;

		int index = 0;
				
		// if this is a preprocessor directive ...
		if(string[0] == '#')
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
				
				// we are at the last character on the line
				if((i == length-1) && (isStringLiteral && string[i] == '\\'))
				{
					// We are in the middle of processing a string literal, but We found an escape character
					token[index-1] = '\0';
					tokenList->addNode(tokenList, token, StringToken);						
				}
				else if(string[i] == '\"')
				{
					// make sure it is not escaped
					if( (string[i-1] != '\\') || ((i>1) && (string[i-2] == '\\')) )
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
				
				if(!isStringLiteral)
				{
//					printf("(i(%d)==(length-1)(%d) && strlen(token)(%d)) == %d\n", i, length, strlen(token), ((i==(length-1)) && strlen(token)));
//					printf("(isWhiteSpace(string[i(%d)])(%d) && strlen(token)(%d)) == %d\n", i, isWhiteSpace(string[i]), strlen(token), (isWhiteSpace(string[i]) && strlen(token)));
					if(((i==(length-1)) && strlen(token)) || (isWhiteSpace(string[i]) && strlen(token)) )
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
			} // for
		} // else -- not a # directive
		node = node->next;
	} // for each char in char *

	DEBUG_PRINT("tokenization directive num: %d\n", tokenNum);

	return 0;
}