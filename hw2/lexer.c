#include "lexer.h"
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
	printf("%s", file_buffer);
	free(file_buffer);
}
