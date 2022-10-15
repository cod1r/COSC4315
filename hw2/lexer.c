#include "lexer.h"
#include <string.h>
bool check_if_keyword(char *token, int len)
{
	if (strcmp(token, "if") == 0) {
	} else if (strcmp(token, "else") == 0) {
	} else if (strcmp(token, "or") == 0) {
	} else if (strcmp(token, "and") == 0) {
	}
	return false;
}
bool check_if_number(char *token, int len)
{
	int counter = 0;
	for (int i = 0; i < len; ++i) {
		if (token[i] < '0' || token[i] > '9') {
			return false;
		}
	}
	return true;
}
word *lexer(char *file_name) {
	FILE *fp = fopen(file_name, "r");
	long long length_of_file = 0;
	char c;
	while ((c = getc(fp)) != EOF) {
		++length_of_file;
	}
	fseek(fp, 0, SEEK_SET);
	unsigned char *file_buffer = malloc(length_of_file);
	long long idx = 0;
	while ((c = getc(fp)) != EOF) {
		file_buffer[idx++] = c;
	}
	word **words = malloc(sizeof(word) * 10);
	int length_of_words = 0;
	for (int i = 0; i < length_of_file;) {
		while (file_buffer[i] == ' ' || file_buffer[i] == '\n' || file_buffer[i] == '\t') {
			++i;
		}
		int start_of_token = i;
		int end_of_token = i;
		while (file_buffer[end_of_token] != ' ' && file_buffer[end_of_token] != '\n' && file_buffer[end_of_token] != '\t') {
				++end_of_token;
		}
		int len = end_of_token - start_of_token;
		char *token = malloc(len);
		for (int token_idx = start_of_token; token_idx < end_of_token; ++token_idx) {
			token[token_idx - start_of_token] = file_buffer[token_idx];
		}
		word *temp = malloc(sizeof(word));
		if (!check_if_keyword(token, len)) {
			if (strcmp(token, "[") == 0) {
				temp->type = LSQBR;
			} else if (strcmp(token, "]") == 0) {
				temp->type = RSQBR;
			} else if (strcmp(token, "\"") == 0) {
				temp->type = QUOTE;
			} else if (check_if_number(token, len)) {
				temp->type = NUMBER;
			} else {
				printf("INVALID TOKEN\n");
				exit(EXIT_FAILURE);
			}
		} else {
			temp->type = KEYWORD;
		}
	}
	free(file_buffer);
}
