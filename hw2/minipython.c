#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
  if (argc < 2) {
    exit(EXIT_FAILURE);
  }
	word *head = lexer(argv[1]);
	while (head != NULL) {
		switch (head->type) {
			case IDENTIFIER: printf("Id: %s\n", head->value); break;
			case PUNCTUATION: printf("Punct: %s\n", head->value); break;
			case NUMBER_LITERAL: printf("Number Lit: %s\n", head->value); break;
			case OPERATOR: printf("Op: %s\n", head->value); break;
		}
		head = head->next;
	}
}
