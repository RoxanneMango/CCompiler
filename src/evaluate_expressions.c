#include "evaluate_expressions.h"

int evaluateExpression(const _StringNode * A, const _StringNode * operator, const _StringNode * B)
{
	
}

int
evaluateExpressions(_LinkedStringList * tokenList, _SyntaxTree * syntaxTree, _Symbol_Table * symbolTable)
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

	// static analysis to make sure there is an even number of opening and closing braces
	int braceEquilibrium = 0;
	_StringNode * node = tokenList->head;
	while(node)
	{
		if(!strcmp(node->data, "(")) braceEquilibrium++;
		else if(!strcmp(node->data, ")")) braceEquilibrium--;
		node = node->next;
	}
	if(braceEquilibrium != 0)
	{
		if(braceEquilibrium > 0) 	  { printf("Unterminated '('\n"); }
		else if(braceEquilibrium < 0) { printf("')' without opening brace\n"); }
		return -1;
	}

	bool isDefinedKeyword = false;
	bool isValidState = true;
	bool isBraceOpened = false;
	
	bool hasBraceOpen = false;

	int encapTokenListInd = 0;
	int encapTokenListInc = 5;
	int encapTokenListMax = 5;
	_LinkedStringList * encapTokenList = calloc(sizeof(_LinkedStringList), encapTokenListMax);
	
	int result = false;


	_StringNode * leftOperand;
	_StringNode * operator;
	_StringNode * rightOperand;

	TokenType prevTokenType = -1;

	node = tokenList->head;
	printf("Node > ");
	tokenList->print(tokenList, false);
	while(node)
	{
/*
		if(!strcmp(node->data, "("))
		{
			hasBraceOpen = true;
		}
		else if(strcmp(node->data, ")"))
		{
			if(!hasBraceOpen)
			{
				DEBUG_PRINT("There was no closing brace!");
				return -1;						
			}
			else
			{
				hasBraceOpen = false;
			}
		}
*/
		
		if(node->type == IdentifierToken)
		{
			if(prevTokenType == IdentifierToken)
			{
				ERROR_PRINT("missing binary operator before token \"%s\"", node->data);
				return -1;
			}
			
			
			// find in symbolTable
			if(!strcmp(node->data, "defined"))
			{
				// find identifier or open brace punctuation
				node = node->next;
				if(!node) return -1;
			}
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
				leftOperand = symbol->filePosition;
				result = true;
			}
			else
			{				
				printf("symbol not found! (%s)\n", node->data);
				result = false;
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
		else if(node->type == PunctuationToken)
		{

		}
		else if(node->type == NumberToken)
		{
			
		}
		else // operators, comparators, assignment
		{
//			printf("%s", node->data);
		}
		prevTokenType = node->type;
		node = node->next;
	}
		
	return result;
}