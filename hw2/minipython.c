#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
  if (argc < 2) {
    exit(EXIT_FAILURE);
  }
  word *head = lexer(argv[1]);
  while (head != NULL) {
    switch (head->type) {
    case IDENTIFIER:
      printf("Id: %s\n", head->value);
      break;
    case PUNCTUATION:
      switch (head->p_type) {
      case INDENT:
        printf("indent: %lu\n", strlen(head->value));
        break;
      case COLON:
        printf("colon: %s\n", head->value);
        break;
      case OPEN_SQR_BR:
        printf("open sqr br\n");
        break;
      case CLOSING_SQR_BR:
        printf("closing sqr br\n");
        break;
      }
      break;
    case NUMBER_LITERAL:
      printf("Number Lit: %s\n", head->value);
      break;
    case OPERATOR:
      printf("Op: %s\n", head->value);
      break;
    case STRING_LITERAL:
      printf("str literal: %s\n", head->value);
      break;
    case KEYWORD:
      printf("keyword: %s\n", head->value);
      break;
    }
    head = head->next;
  }
}
