#include "lexer.h"
#include <string.h>
word *create_word() {
  word *w = malloc(sizeof(word));
  w->value = NULL;
  w->next = NULL;
  return w;
}
bool is_keyword(char *str) {
  for (int i = 0; i < sizeof(KEYWORDS) / sizeof(KEYWORDS[0]); ++i) {
    if (strcmp(KEYWORDS[i], str) == 0) {
      return true;
    }
  }
  return false;
}
bool is_whitespace(char token) {
  if (token == ' ' || token == '\t' || token == '\n') {
    return true;
  }
  return false;
}
bool is_other_token(char token) {
  for (int i = 0; i < sizeof(PUNCTUATIONS) / sizeof(PUNCTUATIONS[0]); ++i) {
    if (token == PUNCTUATIONS[i])
      return true;
  }
  for (int i = 0; i < sizeof(OPERATORS) / sizeof(OPERATORS[0]); ++i) {
    if (token == OPERATORS[i])
      return true;
  }
  return false;
}
bool try_identifier(int *idx, char *buffer, word *res) {
  int copy = *idx;
  if ((buffer[copy] >= '0' && buffer[copy] <= '9') ||
      is_whitespace(buffer[copy]) || is_other_token(buffer[copy]))
    return false;
  ++copy;
  while (copy < strlen(buffer) && !is_other_token(buffer[copy]) &&
         !is_whitespace(buffer[copy])) {
    if (!((buffer[copy] >= '0' && buffer[copy] <= '9') ||
          (buffer[copy] >= 'a' && buffer[copy] <= 'z') ||
          (buffer[copy] >= 'A' && buffer[copy] <= 'Z'))) {
      return false;
    }
    ++copy;
  }
  res->type = IDENTIFIER;
  res->value = malloc(copy - *idx + 1);
  for (int i = 0; i < copy - *idx; ++i) {
    res->value[i] = buffer[i + *idx];
  }
  res->value[copy - *idx] = '\0';
  if (is_keyword(res->value))
    return false;
  *idx = copy;
  return true;
}
bool try_string_literal(int *idx, char *buffer, word *res) {
  if (buffer[*idx] != '\'' && buffer[*idx] != '\"') {
    return false;
  }
  int copy = *idx;
  char single_or_double = buffer[copy];
  ++copy;
  while (copy < strlen(buffer) && buffer[copy] != single_or_double) {
    if (copy + 1 < strlen(buffer) && buffer[copy] == '\\' &&
        buffer[copy + 1] != ' ') {
      copy += 2;
      continue;
    }
    ++copy;
  }
  if (buffer[copy] != single_or_double) {
    printf("ERR string literal not closed\n");
    exit(EXIT_FAILURE);
  }
  ++copy;
  res->type = STRING_LITERAL;
  res->value = malloc(copy - *idx + 1);
  for (int i = 0; i < copy - *idx; ++i) {
    res->value[i] = buffer[i + *idx];
  }
  res->value[copy - *idx] = '\0';
  *idx = copy;
  return true;
}
bool try_number_literal(int *idx, char *buffer, word *res) {
  if (buffer[*idx] < '0' || buffer[*idx] > '9') {
    return false;
  }
  int copy = *idx;
  ++copy;
  while (copy < strlen(buffer) && buffer[copy] >= '0' && buffer[copy] <= '9' &&
         !is_whitespace(buffer[copy])) {
    ++copy;
  }
  res->type = NUMBER_LITERAL;
  res->value = malloc(copy - *idx + 1);
  for (int i = 0; i < copy - *idx; ++i) {
    res->value[i] = buffer[i + *idx];
  }
  res->value[copy - *idx] = '\0';
  *idx = copy;
  return true;
}
bool try_keyword(int *idx, char *buffer, word *res) {
  if (is_whitespace(buffer[*idx]))
    return false;
  int copy = *idx;
  while (copy < strlen(buffer) && !is_other_token(buffer[copy]) &&
         !is_whitespace(buffer[copy])) {
    ++copy;
  }
  char *temp = malloc(copy - *idx + 1);
  for (int i = 0; i < copy - *idx; ++i) {
    temp[i] = buffer[i + *idx];
  }
  temp[copy - *idx] = '\0';
  if (!is_keyword(temp)) {
    free(temp);
    return false;
  }
  res->type = KEYWORD;
  res->value = temp;
  *idx = copy;
  return true;
}
bool try_punctuation(int *idx, char *buffer, word **res_ptr) {
  word *res = *res_ptr;
  for (int i = 0; i < sizeof(PUNCTUATIONS) / sizeof(PUNCTUATIONS[0]); ++i) {
    if (buffer[*idx] == PUNCTUATIONS[i]) {
      res->type = PUNCTUATION;
      switch (PUNCTUATIONS[i]) {
      case ':':
        res->p_type = COLON;
        break;
      case '[':
        res->p_type = OPEN_SQR_BR;
        break;
      case ']':
        res->p_type = CLOSING_SQR_BR;
        break;
      case '\"':
        res->p_type = QUOTE;
        break;
      case '(':
        res->p_type = OPEN_PARENTH;
        break;
      case ')':
        res->p_type = CLOSE_PARENTH;
        break;
      case '.':
        res->p_type = PERIOD;
        break;
      case ',':
        res->p_type = COMMA;
        break;
      case '\'':
        res->p_type = QUOTE;
        break;
      }
      res->value = malloc(2);
      res->value[0] = buffer[*idx];
      res->value[1] = '\0';
      ++(*idx);
      // this part is to tokenize the indent level when we start a new scope
      while (buffer[*idx] == '\n') {
        ++(*idx);
      }
      if (PUNCTUATIONS[i] == ':') {
        word *indent = create_word();
        int indent_idx = *idx;
        char first_whitespace = buffer[indent_idx];
        while (buffer[indent_idx] == first_whitespace &&
               is_whitespace(buffer[indent_idx])) {
          ++indent_idx;
        }
        if (is_whitespace(buffer[indent_idx]) &&
            buffer[indent_idx] != first_whitespace) {
          printf("indentation has a combination of tabs and spaces: %u %u\n",
                 first_whitespace, buffer[indent_idx]);
          exit(EXIT_FAILURE);
        }
        indent->value = malloc(indent_idx - *idx + 1);
        for (int indent_value_idx = 0; indent_value_idx < indent_idx - *idx;
             ++indent_value_idx) {
          indent->value[indent_value_idx] = buffer[indent_value_idx + *idx];
        }
        indent->value[indent_idx - *idx] = '\0';
        indent->type = PUNCTUATION;
        indent->p_type = INDENT;
        res->next = indent;
        *res_ptr = res->next;
        *idx = indent_idx;
      }
      return true;
    }
  }
  return false;
}
bool try_operator(int *idx, char *buffer, word *res) {
  for (int i = 0; i < sizeof(OPERATORS) / sizeof(OPERATORS[0]); ++i) {
    if (buffer[*idx] == OPERATORS[i]) {
      res->type = OPERATOR;
      res->value = malloc(2);
      res->value[0] = buffer[*idx];
      res->value[1] = '\0';
      ++(*idx);
      return true;
    }
  }
  return false;
}
word *lexer(char *file_name) {
  FILE *fp = fopen(file_name, "r");
  if (fp == NULL) {
    printf("file not found");
    exit(EXIT_FAILURE);
  }
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
  int idx_tokens = 0;
  word *head = NULL;
  word *tail = head;
  while (idx_tokens < strlen(file_buffer)) {
    if (tail == NULL && head == NULL) {
      head = create_word();
      tail = head;
    }
    if (try_keyword(&idx_tokens, file_buffer, tail)) {
      tail->next = create_word();
      tail = tail->next;
    } else if (try_identifier(&idx_tokens, file_buffer, tail)) {
      tail->next = create_word();
      tail = tail->next;
    } else if (try_number_literal(&idx_tokens, file_buffer, tail)) {
      tail->next = create_word();
      tail = tail->next;
    } else if (try_string_literal(&idx_tokens, file_buffer, tail)) {
      tail->next = create_word();
      tail = tail->next;
    } else if (try_punctuation(&idx_tokens, file_buffer, &tail)) {
      tail->next = create_word();
      tail = tail->next;
    } else if (try_operator(&idx_tokens, file_buffer, tail)) {
      tail->next = create_word();
      tail = tail->next;
    } else {
      ++idx_tokens;
    }
  }
  word *temp_tail = head;
  while (temp_tail != NULL) {
    if (temp_tail->next == tail) {
      free(temp_tail->next);
      temp_tail->next = NULL;
    }
    temp_tail = temp_tail->next;
  }
  return head;
}
