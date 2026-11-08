#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

enum TokenType
{
	IdentifierToken = 0,
	PunctuationToken,
	OperatorToken,
	ComparisonToken,
	AssignmentToken,
	NumberToken, 
	StringToken,
	DataTypeToken,
	ControlFlowToken,
	StorageClassToken,
	UserDefinedTypeToken,
	QualifierToken,
	PreprocessorToken
};
typedef enum TokenType TokenType;

#endif // TOKEN_TYPE_H