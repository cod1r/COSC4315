#ifndef __WORDS_H
#define __WORDS_H
typedef enum WORD_TYPE
{
	IDENTIFIER,
	KEYWORD,
	VALUE,
} WORD_TYPE;
typedef enum type
{
	NUMBER,
	FLOAT,
	STRING,
	BOOLEAN,
	LIST,
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
typedef struct words
{
	word **wds;
	int length;
} words;
#endif
