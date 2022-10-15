#ifndef __WORDS_H
#define __WORDS_H
typedef enum WORD_TYPE
{
	IDENTIFIER,
	KEYWORD,
	LSQBR,
	RSQBR,
	QUOTE,
	NUMBER,
} WORD_TYPE;
typedef enum type
{
	INT,
	FLOAT,
	STRING,
	BOOLEAN,
} type;
typedef struct word
{
	WORD_TYPE type;
	char *value;
	struct word *next;
} word;
typedef struct value_obj
{
	void *bytes;
} value_obj;
typedef struct list
{
	value_obj *values;
} list;
#endif
