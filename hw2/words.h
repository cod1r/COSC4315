#ifndef __WORDS_H
#define __WORDS_H
enum WORD_TYPE
{
	IDENTIFIER,
	KEYWORD,
	LSQBR,
	RSQBR,
};
typedef struct word
{
	enum WORD_TYPE type;
	char *token;
} word;
#endif
