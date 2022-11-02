#include "lexer.h"
#include <iostream>
#include <string.h>
#include <vector>
bool is_keyword(std::string str) {
  for (int i = 0; i < sizeof(KEYWORDS) / sizeof(KEYWORDS[0]); ++i) {
    if (str == std::string(KEYWORDS[i])) {
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
  if (token == COMMENT_CHAR)
    return true;
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
  res->value = std::string(copy - *idx, '\0');
  for (int i = 0; i < copy - *idx; ++i) {
    res->value[i] = buffer[i + *idx];
  }
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
  res->value = std::string(copy - *idx, '\0');
  for (int i = 0; i < copy - *idx; ++i) {
    res->value[i] = buffer[i + *idx];
  }
  *idx = copy;
  return true;
}
bool try_number_literal(int *idx, char *buffer, word *res) {
  if (buffer[*idx] < '0' or buffer[*idx] > '9') {
    return false;
  }
  int copy = *idx;
  ++copy;
  while (copy < strlen(buffer) and buffer[copy] >= '0' and
         buffer[copy] <= '9' and !is_whitespace(buffer[copy])) {
    ++copy;
  }
  // takes care of floating point numbers
  if (buffer[copy] == '.') {
    ++copy;
    while (copy < strlen(buffer) && buffer[copy] >= '0' &&
           buffer[copy] <= '9' && !is_whitespace(buffer[copy])) {
      ++copy;
    }
  }
  res->type = NUMBER_LITERAL;
  res->value = std::string(copy - *idx, '\0');
  for (int i = 0; i < copy - *idx; ++i) {
    res->value[i] = buffer[i + *idx];
  }
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
  char *temp = (char *)malloc(copy - *idx + 1);
  for (int i = 0; i < copy - *idx; ++i) {
    temp[i] = buffer[i + *idx];
  }
  temp[copy - *idx] = '\0';
  if (!is_keyword(temp)) {
    free(temp);
    return false;
  }
  res->type = KEYWORD;
  res->value = std::string(temp);
  *idx = copy;
  return true;
}
bool try_punctuation(int *idx, char *buffer, word *res) {
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
      res->value = std::string(1, '\0');
      res->value[0] = buffer[*idx];
      ++(*idx);
      return true;
    }
  }
  return false;
}
bool try_operator(int *idx, char *buffer, word *res) {
  for (int i = 0; i < sizeof(OPERATORS) / sizeof(OPERATORS[0]); ++i) {
    if (buffer[*idx] == OPERATORS[i]) {
      res->type = OPERATOR;
      if (OPERATORS[i] == '=') {
        res->o_type = EQL_SIGN;
      } else if (OPERATORS[i] == '+') {
        res->o_type = PLUS_SIGN;
      }
      res->value = std::string(1, '\0');
      res->value[0] = buffer[*idx];
      ++(*idx);
      return true;
    }
  }
  return false;
}
bool try_whitespace(int *idx, char *buffer, word *res) {
  if (!is_whitespace(buffer[*idx]))
    return false;
  int copy = *idx;
  while (is_whitespace(buffer[copy])) {
    ++copy;
  }
  res->value = std::string(copy - *idx, '\0');
  for (int i = 0; i < copy - *idx; ++i) {
    res->value[i] = buffer[i + *idx];
  }
  res->type = WHITESPACE;
  *idx = copy;
  return true;
}
bool try_comment(int *idx, char *buffer, word *res) {
  if (buffer[*idx] != COMMENT_CHAR) {
    return false;
  }
  int copy = *idx;
  while (copy < strlen(buffer) and buffer[copy] != '\n') {
    if (copy + 1 < strlen(buffer) and buffer[copy] == '\\' and
        buffer[copy + 1] == '\n') {
      copy += 2;
      continue;
    }
    ++copy;
  }
  res->value = std::string(copy - *idx, '\0');
  for (size_t i = 0; i < copy - *idx; ++i) {
    res->value[i] = buffer[i + *idx];
  }
  res->type = COMMENT;
  *idx = copy;
  return true;
}
std::vector<word> lexer(char *file_name) {
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
  char *file_buffer = (char *)malloc(length_of_file);
  long long idx = 0;
  while ((c = getc(fp)) != EOF) {
    file_buffer[idx++] = c;
  }
  fclose(fp);
  int idx_tokens = 0;
  std::vector<word> words;
  while (idx_tokens < strlen(file_buffer)) {
    word current;
    if (try_keyword(&idx_tokens, file_buffer, &current)) {
      words.push_back(current);
    } else if (try_identifier(&idx_tokens, file_buffer, &current)) {
      words.push_back(current);
    } else if (try_number_literal(&idx_tokens, file_buffer, &current)) {
      words.push_back(current);
    } else if (try_string_literal(&idx_tokens, file_buffer, &current)) {
      words.push_back(current);
    } else if (try_punctuation(&idx_tokens, file_buffer, &current)) {
      words.push_back(current);
    } else if (try_operator(&idx_tokens, file_buffer, &current)) {
      words.push_back(current);
    } else if (try_whitespace(&idx_tokens, file_buffer, &current)) {
      words.push_back(current);
    } else if (try_comment(&idx_tokens, file_buffer, &current)) {
      words.push_back(current);
    } else {
      ++idx_tokens;
    }
  }
  return words;
}
