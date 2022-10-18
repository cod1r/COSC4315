#ifndef WORDS_H
#define WORDS_H
typedef enum WORD_TYPE
{
	IDENTIFIER,
	KEYWORD,
	PUNCTUATION,
} WORD_TYPE;
typedef enum type
{
	NUMBER,
	FLOAT,
	STRING,
	BOOLEAN,
	LIST,
} type;
typedef struct value_obj
{
	void *bytes;
} value_obj;
typedef struct list
{
	value_obj *values;
} list;
typedef enum PUNCTUATION_TYPE
{
	OPEN_SQR_BR,
	CLOSING_SQR_BR,
	OPEN_PARENTH,
	CLOSE_PARENTH,
	EQL_SIGN,
	PLUS_SIGN,
	COLON,
	QUOTE,
} PUNCTUATION_TYPE;
typedef struct word
{
	WORD_TYPE type;
	char *value;
	struct word *next;
	PUNCTUATION_TYPE p_type;
} word;
typedef struct words
{
	word **wds;
	int length;
} words;
static const char *KEYWORDS[] = {
	"if",
	"elif",
	"else",
	"for",
};
#endif
