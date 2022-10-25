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
	STRING_LITERAL,
	NUMBER_LITERAL,
} WORD_TYPE;
typedef enum PUNCTUATION_TYPE
{
	OPEN_SQR_BR,
	CLOSING_SQR_BR,
	OPEN_PARENTH,
	CLOSE_PARENTH,
	COLON,
	QUOTE,
	COMMA,
	PERIOD,
	INDENT,
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
	'.',
	',',
	'\'',
};
static const char OPERATORS[] = {
	'+',
	'=',
	'*',
	'-',
};
static const char *KEYWORDS[] = {
	"pass",
	"def",
	"if",
	"elif",
	"else",
	"for",
	"True",
	"False",
	"return"
};
word *lexer(char *file_name);
word *create_word();
#endif
