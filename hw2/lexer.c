#include "lexer.h"
#include "words.h"
#include <string.h>
bool check_if_keyword(char *token)
{
	for (int i = 0; i < sizeof(KEYWORDS)/sizeof(KEYWORDS[0]); ++i) {
		if (strcmp(token, KEYWORDS[i]) == 0) {
			return true;
		}
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
	for (int i = 0; i < strlen(token); ++i) {

	}
	return false;
}
void lexer(char *file_name) 
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
	fclose(fp);
	int line_count = 0;
	for (int i = 0; i < length_of_file; ++i) {
		if (file_buffer[i] == '\n') {
			++line_count;
		}
	}
	char **lines = malloc(sizeof(char*) * line_count);
	int current_line = 0;
	int prev = 0;
	for (int i = 0; i < length_of_file; ++i) {
		if (file_buffer[i] == '\n' || i == length_of_file - 1) {
			lines[current_line] = malloc(i - prev + 1);
			for (int j = 0; j < i - prev; ++j) {
				lines[current_line][j] = file_buffer[j + prev];
			}
			lines[current_line][i - prev] = '\0';
			prev = i + 1;
			++current_line;
		}
	}
	for (int i = 0; i < line_count; ++i) {
		printf("%s\n", lines[i]);
	}
}
