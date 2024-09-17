#include "symbol_table.h"

int
initSymbolTable(_Symbol_Table * list)
{
	list->head = NULL;
	list->tail = NULL;
	list->addNode = addSymbolNode;
	list->removeNode = removeSymbolNode;
	list->print = printSymbolTable;
	
	return 0;
}

int 
removeSymbolNode(_Symbol_Table * list, _Symbol * node)
//removeSymbolNode(_Symbol_Table * list, const char * value)
{
	if(!list)
	{
		DEBUG_PRINT("_LinkedStringList * list was NULL\n");
		return -1;
	}
	if(!list->head)
	{
		DEBUG_PRINT("_Symbol_Table head was NULL\n");
		return -1;
	}
	if(!node)
	{
		DEBUG_PRINT("_Symbol * node was NULL\n");
		return -1;
	}
	
	_Symbol * next = node->next;
	_Symbol * previous = node->previous;
	if(node == list->head)
	{
		// head has no previous node!
		if(next)
		{
			list->head = next;
			list->head->previous = NULL;
		}
		// list is now completely empty!
		else
		{
			list->head = NULL;
			list->tail = NULL;
		}
	}
	// symbol is at end of list
	else if(node == list->tail)
	{
		list->tail = previous;
		list->tail->next = NULL;
	}
	// symbol is somewhere in middle of list
	else
	{
		// take out middle node and reconnect the chain
		previous->next = next;
		next->previous = previous;
	}

	// deallocate node
	free(node->name);
	free(node->value);
	free(node);
	
	// decrement list length
	list->length -= 1;
	DEBUG_PRINT("REMOVED!\n");
	
	return 0;
}

int addSymbolNode(_Symbol_Table * list, const char * name, const char * value, _StringNode * filePosition)
{	
	if(!list)
	{
		DEBUG_PRINT("_LinkedStringList * was NULL\n");
		return -1;
	}
	if(!name)
	{
		DEBUG_PRINT("const char * name was NULL\n");
		return -1;
	}
	if(!filePosition)
	{
		DEBUG_PRINT("_StringNode * filePosition was NULL\n");
		return -1;
	}
	
	_Symbol * node = malloc(sizeof(_Symbol));
	if(!node)
	{
		DEBUG_PRINT("Could not add _Symbol node\n");
		return -1;
	}
	
	// assign value name
	int nameLen = strlen(name) + 1;
	node->name = malloc(sizeof(char) * nameLen);
	node->name[0] = '\0';
	node->index = 0;
	strcpy(node->name, name);

	node->fileName = NULL;
	node->filePosition = filePosition;

	DEBUG_PRINT("addSymbolNode() -> symbol name: %s\n", node->name);

	// set symbol to be defined
	node->isDefined = true;

	// if symbol has a value assign it
	if(value)
	{
		int valueLen = strlen(value) + 1;
		node->value = malloc(sizeof(char) * valueLen);
		node->value[0] = '\0';
		strcpy(node->value, value);
		
		DEBUG_PRINT("addSymbolNode() -> symbol value: %s\n", node->value);
	}
	else
	{
		node->value = NULL;
	}
	
	node->next = NULL;
	// List has no nodes yet, make this the first node
	if(!list->head)
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

int printSymbolTable(_Symbol_Table * list)
{
	if(!list)
	{
		DEBUG_PRINT("_Symbol_Table * was NULL\n");
		return -1;
	}
	if(!list->head)
	{
		DEBUG_PRINT("_Symbol_Table head was NULL\n");
		return -1;		
	}
	
	DEBUG_PRINT("list length: %d\n", list->length);
	
	_Symbol * node = list->head;
	while(node)
	{
		printf("%d\t| %s %s\n", node->index, node->name, node->value?node->value:"");
		node = node->next;
	}
	return 0;
}

_Symbol * 
findSymbolNode(_Symbol_Table * list, const char * name)
{
	if(!list)
	{
		DEBUG_PRINT("_Symbol_Table * was NULL\n");
		return NULL;
	}
	if(!list->head)
	{
		DEBUG_PRINT("_Symbol_Table head was NULL\n");
		return NULL;
	}	
	DEBUG_PRINT("list length: %d\n", list->length);
	
	_Symbol * node = list->head;
	while(node)
	{
		if(node->name && strcmp(node->name, name) == 0)
		{			
			// found the node!
			DEBUG_PRINT("FOUND _Symbol : %s!\n", node->name);
			return node;
		}
		node = node->next;
	}
	return NULL;
}


int 
initSymbolPointerList(_Symbol_Pointer_List * list)
{
	if(!list)
	{
		DEBUG_PRINT("_Symbol_Pointer_List * was NULL\n");
		return -1;
	}
		
	list->length = 0;
	list->head = NULL;
	list->tail = NULL;
	list->addNode = addSymbolPointerNode;
	list->removeNode = removeSymbolPointerNode;
	list->print = printSymbolPointerList;
	
	return 0;
}



int 
addSymbolPointerNode(_Symbol_Pointer_List * list, _Symbol * symbol)
{
	if(!list)
	{
		DEBUG_PRINT("_Symbol_Pointer_List * was NULL\n");
		return -1;
	}
	if(!symbol)
	{
		DEBUG_PRINT("_Symbol_Pointer_List_Node * node was NULL\n");
		return -1;
	}

	_Symbol_Pointer_List_Node * node = malloc(sizeof(_Symbol_Pointer_List_Node));
	if(!node)
	{
		DEBUG_PRINT("Could not add _Symbol_Pointer_List node\n");
		return -1;
	}

	// assign value name
	node->index = 0;
	node->symbol = symbol;

	DEBUG_PRINT("addSymbolNode() -> symbol  name: %s\n", symbol->name);
	
	node->next = NULL;
	// List has no nodes yet, make this the first node
	if(!list->head)
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
int 
removeSymbolPointerNode(_Symbol_Pointer_List * list, _Symbol_Pointer_List_Node * node)
{
	if(!list)
	{
		DEBUG_PRINT("_Symbol_Pointer_List * was NULL\n");
		return -1;
	}
	if(!node)
	{
		DEBUG_PRINT("_Symbol_Pointer_List_Node * node was NULL\n");
		return -1;
	}

	_Symbol_Pointer_List_Node * next = node->next;
	_Symbol_Pointer_List_Node * previous = node->previous;

	if(node == list->head)	// its the start node
	{
		list->head = NULL;
		list->tail = NULL;
	}
	else if(next)			// its a node in the middle of the list
	{
		previous->next = next;
		next->previous = previous;
	}
	else					// its at the end of the list
	{
		previous->next = NULL;
	}
	free(node);
	
	return 0;
}
int 
printSymbolPointerList(_Symbol_Pointer_List * list)
{
	if(!list)
	{
		DEBUG_PRINT("_Symbol_Pointer_List * was NULL\n");
		return -1;
	}
	if(!list->head)
	{
		DEBUG_PRINT("_Symbol_Pointer_List_Node * head node was NULL (list was empty)\n");
		return -1;
	}

	DEBUG_PRINT("printSymbolPointerList length: %d\n", list->length);
	
	_Symbol_Pointer_List_Node * node = list->head;
	while(node)
	{
		printf("%d\t| %s %s\n", node->symbol->index, node->symbol->name, node->symbol->value?node->symbol->value:"");
		node = node->next;
	}
	
	return 0;
}