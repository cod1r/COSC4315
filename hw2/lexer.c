#include "lexer.h"
#include <string.h>
bool check_if_keyword(char *token)
{
	if (strcmp(token, "if") == 0
	|| strcmp(token, "else") == 0
	|| strcmp(token, "or") == 0
	|| strcmp(token, "and") == 0
	|| strcmp(token, "[") == 0
	|| strcmp(token, "]") == 0
	|| strcmp(token, "\"") == 0
	|| strcmp(token, "=") == 0
	|| strcmp(token, "+") == 0
	|| strcmp(token, "(") == 0
	|| strcmp(token, ")") == 0) {
		return true;
	}
	return false;
}
// TODO: we need to check for decimal
bool check_if_number(char *token)
{
	for (int i = 0; i < strlen(token); ++i) {
		if (token[i] < '0' || token[i] > '9') {
			return false;
		}
	}
	return true;
}
bool check_if_proper_identifier(char *token)
{
	if (token[0] >= '0' && token[0] <= '9') return false;
	for (int i = 0; i < strlen(token); ++i) {
		if ((token[i] < 'a' || token[i] > 'z') && (token[i] < '0' || token[i] > '9') && (token[i] < 'A' || token[i] > 'Z'))
			return false;
	}
	return true;
}
bool check_if_valid_value(char *token)
{
	return false;
}
words lexer(char *file_name) 
{
	FILE *fp = fopen(file_name, "r");
	long long length_of_file = 0;
	char c;
	while ((c = getc(fp)) != EOF) {
		++length_of_file;
	}
	fseek(fp, 0, SEEK_SET);
	char *file_buffer = malloc(length_of_file);
	long long idx = 0;
	while ((c = getc(fp)) != EOF) {
		file_buffer[idx++] = c;
	}
	int length_of_words = 0;
	int capacity_of_words = 10;
	words wds;
	word **words = malloc(sizeof(word) * capacity_of_words);
	wds.wds = words;
	for (int i = 0; i < length_of_file;) {
		while (file_buffer[i] == ' ' || file_buffer[i] == '\n' || file_buffer[i] == '\t') {
			++i;
		}
		int start_of_token = i;
		int end_of_token = i;
		while (!check_if_keyword(&file_buffer[start_of_token])
				&& end_of_token < length_of_file
				&& file_buffer[end_of_token] != ' '
				&& file_buffer[end_of_token] != '\n'
				&& file_buffer[end_of_token] != '\t') {
				++end_of_token;
		}
		// adding 1 bc of the \0
		int len = end_of_token - start_of_token + 1;
		char *token = malloc(len);
		for (int token_idx = start_of_token; token_idx < end_of_token; ++token_idx) {
			token[token_idx - start_of_token] = file_buffer[token_idx];
		}
		token[end_of_token] = '\0';
		word *temp = malloc(sizeof(word));
		if (!check_if_keyword(token) && check_if_proper_identifier(token)) {
			temp->type = IDENTIFIER;
		} else if (check_if_keyword(token)) {
			temp->type = KEYWORD;
		} else if (check_if_valid_value(token)) {
			temp->type = VALUE;
		} else {
		}

		words[length_of_words++] = temp;
		if (length_of_words == capacity_of_words) {
			word **temp_words = malloc(sizeof(word) * length_of_words);
			for (int i = 0; i < length_of_words; ++i) {
				temp_words[i] = words[i];
			}
			free(words);
			words = malloc(sizeof(word) * (length_of_words + 10));
			for (int i = 0; i < length_of_words; ++i) {
				words[i] = temp_words[i];
			}
			free(temp_words);
		}
		i = end_of_token;
	}
	free(file_buffer);
	return wds;
}
