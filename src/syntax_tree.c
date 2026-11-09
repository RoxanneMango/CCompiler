#include "syntax_tree.h"
#include "string_utils.h"

#include <string.h>

_SyntaxTree syntaxTree_PreprocessorDirectives =
{
	.size = 0,
	.children = NULL,
	.add = addSyntaxTreeNode,
	.search = searchSyntaxTreeNode
};
_SyntaxTree syntaxTree_punctuation_and_operators =
{
	.size = 0,
	.children = NULL,
	.add = addSyntaxTreeNode,
	.search = searchSyntaxTreeNode
};
_SyntaxTree syntaxTree_keywords =
{
	.size = 0,
	.children = NULL,
	.add = addSyntaxTreeNode,
	.search = searchSyntaxTreeNode
};

int 
_initSyntaxTreeNode(_SyntaxTreeNode * node, bool isLeaf, const int depth, const int size, char data, TokenType tokenType)
{	
	node->isLeaf = isLeaf;
	node->depth = depth;
	node->size = size;
	node->data = data;
	node->type = tokenType;
	node->children = NULL;
	
	DEBUG_PRINT("_InitSyntaxTreeNode '%c' isLeaf=%s\n", data, isLeaf ? "true" : "false");
	
	return 0;
}

int 
_initSyntaxTree(_SyntaxTree * syntaxTree)
{
	syntaxTree->size = 0;
	syntaxTree->children = NULL;
	syntaxTree->add = addSyntaxTreeNode;
	syntaxTree->search = searchSyntaxTreeNode;
}

_SyntaxTreeSearchResult 
searchSyntaxTreeNode(_SyntaxTree * syntaxTree, const char * string)
{
	_SyntaxTreeSearchResult result = { .index = -1, .type = IdentifierToken };
	
	// error checking
	if(!syntaxTree) { DEBUG_PRINT("_SyntaxTree * tree was NULL\n"); 	return result; }
	if(!string) 	{ DEBUG_PRINT("char * str was NULL\n"); 			return result; }

	int length = strlen(string);
	if(!length)
	{
		DEBUG_PRINT("char * str was empty!\n"); return result;
	}

	DEBUG_PRINT("searched string: %s\n", string);

	int index = 0;
	_SyntaxTreeNode *** list = &syntaxTree->children;

	bool isFound = *list == NULL ? false : true;
	int listSize = syntaxTree->size;
	while(isFound && (index < length))
	{
		isFound = false;
		bool isLeaf = false;
		DEBUG_PRINT("\t\tTrying to match letter '%c' at index %d with node ...\n\t\t\tLetters: ", string[index], index);		
		for(int child_i = 0; (*list) && child_i < listSize; child_i++)
		{
			_SyntaxTreeNode * node = (*list)[child_i];
//			DEBUG_PRINT("'%c', ", node->data);
//			DEBUG_PRINT("== '%c'\n", string[index]);
			if(node->data == string[index])
			{
				DEBUG_PRINT("... \n\t\tMatched letter '%c' with node %d:%d! \n", string[index], index, child_i);

				list = &node->children;
				listSize = node->size;
				isFound = true;

				index += 1;

				if(node->isLeaf)
				{
					DEBUG_PRINT("is leaf result : %s\n", string);
					result.index = index;
					result.type = node->type;
				}
				break;
			}
		}
		if(!isFound)
		{
			DEBUG_PRINT("\n\t\tCould not find a match!\n");
		}
	}	
	return result;
}

int 
addSyntaxTreeNode(_SyntaxTree * syntaxTree, const char * string, const TokenType tokenType)
{
	DEBUG_PRINT("int addSyntaxTreeNode(_SyntaxTree * = '%s', char * str = '%s')\n", "tree", string);
	DEBUG_PRINT("==============================================================================================\n");
	
	// error checking
	if(!syntaxTree) 	{ DEBUG_PRINT("_SyntaxTree * tree was NULL\n"); 	return -1; }
	if(!string) { DEBUG_PRINT("char * str was NULL\n"); 					return -1; }

	int index = 0;
	int length = strlen(string);

	_SyntaxTreeNode * node = NULL;
	_SyntaxTreeNode *** list = &syntaxTree->children;

	// if the tree is empty, create the first node
	DEBUG_PRINT("Checking to see if tree is empty ...\n");
	if(!*list)
	{
		DEBUG_PRINT("\tTree was empty -- adding '%c' and advancing ...\n", string[index]);

		(*list) = malloc(sizeof(_SyntaxTreeNode*));
		(*list)[0] = malloc(sizeof(_SyntaxTreeNode));
		
		_initSyntaxTreeNode(((*list)[0]), index>=(length-1), 1, 0, string[index], index>=(length-1) ? tokenType : IdentifierToken);

		node = (*list)[0];
		list = &node->children;

		syntaxTree->size = 1;

		index++;
	}
	else
	{
		DEBUG_PRINT("\tTree was was not empty\n");
	}

	int listSize = syntaxTree->size;
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
			DEBUG_PRINT("'%c', ", list && *list ? (*list)[child_i]->data : 'X'); 
			if(list && *list && (*list)[child_i]->data == string[index])
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
	DEBUG_PRINT("Was there a complete match with the string? -- %s --", isFound ? "true" : "false");
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
			syntaxTree->size++;
		}
		listSize++;
	}
	else
	{
		DEBUG_PRINT("\tMade node into leaf\n");		
		node->isLeaf = true;
		node->type = tokenType;
	}
	
	// the node is not in the list, so add it!
	DEBUG_PRINT("if((index(=%d) <= length(=%d)) && *list) ... %s\n", index, length, ((index <= length) && *list) ? "true -- entering loop" : "false -- skipping");
	if((index < length) && *list)
	{
		DEBUG_PRINT("\tCould not match letter '%c' with node at depth %d -- making list bigger (%d) and adding letter '%c'... \n", string[index], index, listSize, string[index]);
		(*list) = realloc((*list), sizeof(_SyntaxTreeNode*) * listSize);
		(*list)[listSize-1] = malloc(sizeof(_SyntaxTreeNode));
		_initSyntaxTreeNode((*list)[listSize-1], index>=(length-1), index, 0, string[index], index>=(length-1) ? tokenType : IdentifierToken);
		(*list)[listSize-1]->size += ((index+1) < length);
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
		_initSyntaxTreeNode(((*list)[0]), index>=(length-1), 1, 0, string[index], index>=(length-1) ? tokenType : IdentifierToken);
		(*list)[0]->size = 1;
		list = &(*list)[0]->children;
		index++;
	}
	
//	DEBUG_PRINT("Tree child nodes: %d\n\t", tree->size);
//	for(int i = 0; i < tree->size; i++)
//	{
//		DEBUG_PRINT("'%c' ; isLeaf: %s", tree->children[i]->data, (tree->children[i]->isLeaf ? "true" : "false"));
//	}
	DEBUG_PRINT("\n==============================================================================================\n");

	return 0;
}