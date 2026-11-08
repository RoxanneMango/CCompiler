#ifndef PREPROCESSOR_DIRECTIVES_H
#define PREPROCESSOR_DIRECTIVES_H

#include "string_utils.h"
#include "symbol_table.h"
#include "syntax_tree.h"

extern const char * LINE_MACRO;
extern const char * FILE_MACRO;

// Handle '#' statements
int handlePreprocessorDirectives(_LinkedStringList * fileList, _SyntaxTree * syntaxTree, _Symbol_Table * symbolTable);

// Remove comments from the code
int filterOutComments(_LinkedStringList * list);
// Remove segments of code during conditional compilation
int removeBetweenSymbols(_Symbol_Table * list, _LinkedStringList * fileList, _Symbol * begin);

// return the node where the preprocessing will continue
// return NULL if something went wrong during processing
void handleIfGroups(_Symbol_Table * list, _LinkedStringList * fileList, _Symbol_Table * symbolTable, _Symbol * directive);

// Parse the #include directives
int handleIncludeDirective(_LinkedStringList * tokenList);
// Follow included files and add them to be processed
int expandIncludes(_LinkedStringList * list);

#endif // PREPROCESSOR_DIRECTIVES_H