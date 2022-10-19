#ifndef LEXER_H
#define LEXER_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
typedef enum WORD_TYPE
{
	IDENTIFIER,
	KEYWORD,
	PUNCTUATION,
	OPERATOR,
} WORD_TYPE;
typedef enum PUNCTUATION_TYPE
{
	OPEN_SQR_BR,
	CLOSING_SQR_BR,
	OPEN_PARENTH,
	CLOSE_PARENTH,
	COLON,
	QUOTE,
} PUNCTUATION_TYPE;
typedef enum OPERATOR_TYPE
{
	EQL_SIGN,
	PLUS_SIGN,
} OPERATOR_TYPE;
typedef struct word
{
	WORD_TYPE type;
	char *value;
	struct word *next;
	PUNCTUATION_TYPE p_type;
	OPERATOR_TYPE o_type;
} word;
static const char PUNCTUATIONS[] = {
	':',
	'[',
	']',
	'\"',
	'(',
	')',
};
static const char OPERATORS[] = {
	'+',
	'=',
};
static const char *KEYWORDS[] = {
	"if",
	"elif",
	"else",
	"for",
};
void lexer(char *file_name);
#endif
