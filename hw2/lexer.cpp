#include "lexer.h"
#include <iostream>
#include <string.h>
#include <string>
#include <vector>
bool is_keyword(std::string str) {
  for (int i = 0; i < KEYWORDS.size(); ++i) {
    if (str == KEYWORDS[i]) {
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
bool is_other_token(size_t start, char *buffer) {
  for (size_t i = 0; i < PUNCTUATIONS.size(); ++i) {
    if (start + PUNCTUATIONS[i].length() - 1 >= strlen(buffer)) {
      continue;
    }
    std::string part = std::string(PUNCTUATIONS[i].length(), '\0');
    for (size_t part_idx = 0; part_idx < PUNCTUATIONS[i].length(); ++part_idx) {
      part[part_idx] = buffer[start + part_idx];
    }
    if (part == PUNCTUATIONS[i]) {
      return true;
    }
  }
  for (size_t i = 0; i < OPERATORS.size(); ++i) {
    if (start + OPERATORS[i].length() - 1 >= strlen(buffer)) {
      continue;
    }
    std::string part = std::string(OPERATORS[i].length(), '\0');
    for (size_t part_idx = 0; part_idx < OPERATORS[i].length(); ++part_idx) {
      part[part_idx] = buffer[start + part_idx];
    }
    if (part == OPERATORS[i]) {
      return true;
    }
  }
  if (buffer[start] == COMMENT_CHAR) {
    return true;
  }
  return false;
}
bool try_identifier(size_t *idx, char *buffer, word *res) {
  size_t copy = *idx;
  if ((buffer[copy] >= '0' && buffer[copy] <= '9') ||
      is_whitespace(buffer[copy]) || is_other_token(copy, buffer))
    return false;
  ++copy;
  while (copy < strlen(buffer) && !is_other_token(copy, buffer) &&
         !is_whitespace(buffer[copy])) {
    if (!((buffer[copy] >= '0' && buffer[copy] <= '9') ||
          (buffer[copy] >= 'a' && buffer[copy] <= 'z') ||
          (buffer[copy] >= 'A' && buffer[copy] <= 'Z'))) {
      return false;
    }
    ++copy;
  }
  if (is_other_token(copy - 1, buffer)) {
    return false;
  }
  res->type = IDENTIFIER;
  res->value = std::string(copy - *idx, '\0');
  for (size_t i = 0; i < copy - *idx; ++i) {
    res->value[i] = buffer[i + *idx];
  }
  if (is_keyword(res->value)) {
    return false;
  }
  *idx = copy;
  return true;
}
bool try_string_literal(size_t *idx, char *buffer, word *res) {
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
bool try_number_literal(size_t *idx, char *buffer, word *res) {
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
bool try_keyword(size_t *idx, char *buffer, word *res) {
  if (is_whitespace(buffer[*idx]))
    return false;
  size_t copy = *idx;
  while (copy < strlen(buffer) and !is_other_token(copy, buffer) and
         !is_whitespace(buffer[copy])) {
    ++copy;
  }
  char *temp = (char *)malloc(copy - *idx + 1);
  for (int i = 0; i < copy - *idx; ++i) {
    temp[i] = buffer[i + *idx];
  }
  temp[copy - *idx] = '\0';
  std::string temp_str = std::string(temp);
  if (!is_keyword(temp_str)) {
    free(temp);
    return false;
  }
  res->type = KEYWORD;
  res->value = std::string(temp);
  *idx = copy;
  return true;
}
bool try_punctuation(size_t *idx, char *buffer, word *res) {
  for (size_t i = 0; i < PUNCTUATIONS.size(); ++i) {
    if (*idx + PUNCTUATIONS[i].length() - 1 >= strlen(buffer)) {
      continue;
    }
    std::string part = std::string(PUNCTUATIONS[i].length(), '\0');
    for (size_t temp_idx = 0; temp_idx < PUNCTUATIONS[i].length(); ++temp_idx) {
      part[temp_idx] = buffer[temp_idx + *idx];
    }
    if (part == PUNCTUATIONS[i]) {
      res->type = PUNCTUATION;
      if (PUNCTUATIONS[i] == ":") {
        res->p_type = COLON;
      } else if (PUNCTUATIONS[i] == "[") {
        res->p_type = OPEN_SQR_BR;
      } else if (PUNCTUATIONS[i] == "]") {
        res->p_type = CLOSING_SQR_BR;
      } else if (PUNCTUATIONS[i] == "\"") {
        res->p_type = QUOTE;
      } else if (PUNCTUATIONS[i] == "(") {
        res->p_type = OPEN_PARENTH;
      } else if (PUNCTUATIONS[i] == ")") {
        res->p_type = CLOSE_PARENTH;
      } else if (PUNCTUATIONS[i] == ".") {
        res->p_type = PERIOD;
      } else if (PUNCTUATIONS[i] == ",") {
        res->p_type = COMMA;
      } else if (PUNCTUATIONS[i] == "\'") {
        res->p_type = QUOTE;
      }
      res->value = part;
      (*idx) += part.length();
      return true;
    }
  }
  return false;
}
bool try_operator(size_t *idx, char *buffer, word *res) {
  for (size_t i = 0; i < OPERATORS.size(); ++i) {
    if (*idx + OPERATORS[i].length() >= strlen(buffer)) {
      continue;
    }
    std::string part = std::string(OPERATORS[i].length(), '\0');
    for (size_t temp_idx = 0; temp_idx < OPERATORS[i].length(); ++temp_idx) {
      part[temp_idx] = buffer[temp_idx + *idx];
    }
    if (part == OPERATORS[i]) {
      res->type = OPERATOR;
      if (OPERATORS[i] == "=") {
        res->o_type = EQL_SIGN;
      } else if (OPERATORS[i] == "+") {
        res->o_type = PLUS_SIGN;
      } else if (OPERATORS[i] == "-") {
        res->o_type = SUBTRACT;
      } else if (OPERATORS[i] == "==") {
        res->o_type = EQUAL_BOOL;
      } else if (OPERATORS[i] == "!=") {
        res->o_type = NOT_EQUAL_TO;
      }
      res->value = part;
      (*idx) += part.length();
      return true;
    }
  }
  return false;
}
bool try_whitespace(size_t *idx, char *buffer, word *res) {
  if (!is_whitespace(buffer[*idx]))
    return false;
  size_t copy = *idx;
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
bool try_comment(size_t *idx, char *buffer, word *res) {
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
  size_t idx_tokens = 0;
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
