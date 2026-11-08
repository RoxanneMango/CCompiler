#ifndef EVALUATE_EXPRESSIONS_H
#define EVALUATE_EXPRESSIONS_H

#define EVAL(X) (X)==0

#include "string_utils.h"
#include "symbol_table.h"
#include "syntax_tree.h"

int evaluateExpression(const _StringNode * A, const _StringNode * operator, const _StringNode * B);
int evaluateExpressions(_LinkedStringList * tokenList, _SyntaxTree * syntaxTree, _Symbol_Table * symbolTable);

#endif // EVALUATE_EXPRESSIONS_H