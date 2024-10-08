#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stdlib.h>	// standard memory operations, etc
#include <stdio.h>  // standard IO
#include <string.h>	// string manipulations
#include <stdbool.h>// true false bool definition
#include <dirent.h> // DIR * definition

#ifndef DEBUG
#define DEBUG 0
#else
#define DEBUG 1
#endif
#define DEBUG_PRINT(format, ...) if(DEBUG) printf(format, ##__VA_ARGS__)

struct _StringNode typedef _StringNode;
struct _LinkedStringList typedef _LinkedStringList;

struct _StringNode;
struct _LinkedStringList;

enum TokenType
{
	IdentifierToken = 90, // dark gray
	PunctuationToken = 94, // blue
	OperatorToken = 96, // cyan
	ComparisonToken = 91, // red
	AssignmentToken = 95, // magenta
	NumberToken = 92, // green 
	StringToken = 33, // dark yellow
	PreprocessorToken = 35 // dark magenta
};
typedef enum TokenType TokenType;

int addStringNode(_LinkedStringList * list, const char * string, TokenType type);
int removeStringNode(_LinkedStringList * list, _StringNode * node);
int printStringList(_LinkedStringList * list, bool prettyPrint);
int printStringListSimple(_LinkedStringList * list);
int initStringList(_LinkedStringList * list);
//
int stitchTogether(_LinkedStringList * list);
bool partOfString(char * line, char * substr);
bool isWhiteSpace(const char c);
bool isOnlyWhiteSpaces(char * line);
int trimWhiteSpaces(char * string);

struct _StringNode
{
	int index;
	int length;
	char * data;
	
	TokenType type;
	
	struct _StringNode * previous;
	struct _StringNode * next;
};

struct _LinkedStringList
{
	unsigned int length;
	_StringNode * head;
	_StringNode * tail;
	int(*addNode)(_LinkedStringList * list, const char * string, TokenType type);
	int(*removeNode)(_LinkedStringList * list, _StringNode * node);
	int(*print)(_LinkedStringList * list, bool prettyPrint);
};

#endif // STRING_UTILS_H