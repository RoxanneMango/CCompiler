#ifndef SYNTAX_TREE_H
#define SYNTAX_TREE_H

#include "token_type.h"

#include <stdbool.h>
#include <stddef.h>

struct _SyntaxTreeSearchResult
{
	int index;
	TokenType type;
};
struct _SyntaxTreeSearchResult typedef _SyntaxTreeSearchResult;

struct _SyntaxTreeNode
{
	int isLeaf;
	int depth;
	int size;
	char data;
	TokenType type;
	struct _SyntaxTreeNode ** children;
};
struct _SyntaxTreeNode typedef _SyntaxTreeNode;

struct _SyntaxTree
{
	int size;
	_SyntaxTreeNode ** children;

	int (*add)(struct _SyntaxTree * syntaxTree, const char * string, const TokenType tokenType);
	_SyntaxTreeSearchResult (*search)(struct _SyntaxTree * syntaxTree, const char * string);
};
struct _SyntaxTree typedef _SyntaxTree;

int _initSyntaxTreeNode(_SyntaxTreeNode * node, bool isLeaf, const int depth, const int size, char data, const TokenType tokenType);
int _initSyntaxTree(_SyntaxTree * syntaxTree);
int addSyntaxTreeNode(_SyntaxTree * syntaxTree, const char * str, const TokenType tokenType);
_SyntaxTreeSearchResult searchSyntaxTreeNode(_SyntaxTree * syntaxTree, const char * string);

extern _SyntaxTree syntaxTree_PreprocessorDirectives;

#endif // SYNTAX_TREE_H