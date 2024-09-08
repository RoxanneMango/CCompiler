#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "string_utils.h"

struct _Symbol typedef _Symbol;
struct _Symbol_Table typedef _Symbol_Table;
struct _Symbol_Pointer_List_Node typedef _Symbol_Pointer_List_Node;
struct _Symbol_Pointer_List typedef _Symbol_Pointer_List;

struct _Symbol;
struct _Symbol_Table;

struct _Symbol
{
	int index;
	bool isDefined;
	char * name;
	char * value;

	char * fileName;
	_StringNode * filePosition;

	struct _Symbol * previous;
	struct _Symbol * next;
};

struct _Symbol_Table
{
	unsigned int length;
	_Symbol * head;
	_Symbol * tail;
	
	_Symbol_Table * ifStack;
	
	int(*addNode)(_Symbol_Table * list, const char * name, const char * value, _StringNode * filePosition);
	int(*removeNode)(_Symbol_Table * list, _Symbol * node);
	int(*print)(_Symbol_Table * list);
	int(*find)(_Symbol_Table * list, const char * name, const char * value);
};

struct _Symbol_Pointer_List_Node
{
	int index;
	_Symbol * symbol;
	
	_Symbol_Pointer_List_Node * next;
	_Symbol_Pointer_List_Node * previous;
};

struct _Symbol_Pointer_List
{
	unsigned int length;
	_Symbol_Pointer_List_Node * head;
	_Symbol_Pointer_List_Node * tail;
	
	int(*addNode)(_Symbol_Pointer_List * list, _Symbol * node);
	int(*removeNode)(_Symbol_Pointer_List * list, _Symbol_Pointer_List_Node * node);
	int(*print)(_Symbol_Pointer_List * list);
};

int addSymbolPointerNode(_Symbol_Pointer_List * list, _Symbol * node);
int removeSymbolPointerNode(_Symbol_Pointer_List * list, _Symbol_Pointer_List_Node * node);
int printSymbolPointerList(_Symbol_Pointer_List * list);
int initSymbolPointerList(_Symbol_Pointer_List * list);

int addSymbolNode(_Symbol_Table * list, const char * name, const char * value, _StringNode * filePosition);
int removeSymbolNode(_Symbol_Table * list, _Symbol * node);
int printSymbolTable(_Symbol_Table * list);
int findSymbolNode(_Symbol_Table * list, const char * name, const char * value);

int initSymbolTable(_Symbol_Table * list);

#endif // SYMBOL_TABLE_H