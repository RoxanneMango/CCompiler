#ifndef TOKENIZATION_H
#define TOKENIZATION_H

#include "string_utils.h"
#include "syntax_tree.h"

static char * _Punctuation[] = 
{
	".", ",", ";", ":",
	"(", ")", "{", "}",
	"[", "]", "'", "\""
};

static char * _Operators[] = 
{
	// operators
	"!", "+", "-", "=",
	"<", ">", "*", "/",
	"&", "|", "%", "^",
	"~", "#", "?",
	// double operators
	"--", "++", "||", "&&", "<<", ">>", "##", "->",
	"-=", "+=", "|=", "&=", "<=", ">=", "==", "!=",
	"^=", "*=", "/=", "%=", "<<=", ">>="
};

static char * _DataTypes[] =
{
	"int", "short", "long", "signed", "unsigned", 
	"void", "char", "double", "float", 
	"struct", "union"
};

static char * _UserDefinedTypes[] =
{
	"struct", "union", "enum", "typedef"
};

static char * _ControlFlow[] =
{
	"if", "else", "switch", "case", "default", 
	"for", "while", "do", "break", "continue", 
	"return", "goto"
};

static char * _StorageClass[] =
{
	"auto", "register", "static", "extern"
};

static char * _Qualifier[] =
{
	"const", "volatile", "restrict"
};



static char * _PreprocessorIdentifiers[] =
{
	"include", "ifndef", "pragma", "define", "endif", 
	"ifdef", "undef", "error", "elif", "else", "line", 
	"if", "__LINE__", "__FILE__"
};


int assignProcessedTokens(_LinkedStringList * tokenList, _SyntaxTree * syntaxTree);

int processToken(char * string, _LinkedStringList * tokenList, _SyntaxTree * syntaxTree);
int lineTokenization(char * string, _LinkedStringList * tokenList, _SyntaxTree * syntaxTree);
int tokenization(_LinkedStringList * list, _LinkedStringList * tokenList, _SyntaxTree * syntaxTree);

#endif // TOKENIZATION_H