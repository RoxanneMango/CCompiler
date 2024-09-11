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

int addStringNode(_LinkedStringList * list, const char * string);
int removeStringNode(_LinkedStringList * list, _StringNode * node);
int printStringList(_LinkedStringList * list, bool prettyPrint);
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
	
	struct _StringNode * previous;
	struct _StringNode * next;
};

struct _LinkedStringList
{
	unsigned int length;
	_StringNode * head;
	_StringNode * current;
	int(*addNode)(_LinkedStringList * list, const char * string);
	int(*removeNode)(_LinkedStringList * list, _StringNode * node);
	int(*print)(_LinkedStringList * list, bool prettyPrint);
};

#endif // STRING_UTILS_H