#include "lexer.h"
#include <string.h>
bool check_if_keyword(char *token) {
  for (int i = 0; i < sizeof(KEYWORDS) / sizeof(KEYWORDS[0]); ++i) {
    if (strcmp(token, KEYWORDS[i]) == 0) {
      return true;
    }
  }
  return false;
}
// TODO: we need to check for decimal
bool check_if_number(char *token) {
  for (int i = 0; i < strlen(token); ++i) {
    if (token[i] < '0' || token[i] > '9') {
      return false;
    }
  }
  return true;
}
bool check_if_proper_identifier(char *token) {
  if (token[0] >= '0' && token[0] <= '9')
    return false;
  for (int i = 0; i < strlen(token); ++i) {
    if ((token[i] < 'a' || token[i] > 'z') &&
        (token[i] < '0' || token[i] > '9') &&
        (token[i] < 'A' || token[i] > 'Z'))
      return false;
  }
  return true;
}
bool check_if_valid_value(char *token) {
  for (int i = 0; i < strlen(token); ++i) {
  }
  return false;
}
void lexer(char *file_name) {
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
  word **line_words = malloc(sizeof(word *) * line_count);
  char **lines = malloc(sizeof(char *) * line_count);
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
  // PROCESSING...
  for (int i = 0; i < line_count; ++i) {
    word *word_line = malloc(sizeof(word));
    word *tail = word_line;
    for (int j = 0; j < strlen(lines[i]); ++j) {
      switch (lines[i][j]) {
      case '=': {
        int begin = 0;
        int end = j - 1;
        while (begin < strlen(lines[i]) &&
               (lines[i][begin] == ' ' || lines[i][begin] == '\t' ||
                lines[i][begin] == '\n')) {
          ++begin;
        }
        while (end >= 0 && (lines[i][end] == ' ' || lines[i][end] == '\t' ||
                            lines[i][end] == '\n')) {
          --end;
        }
        char *ident = malloc(end - begin + 2);
        for (int idx = begin; idx <= end; ++idx) {
          ident[idx] = lines[i][idx];
        }
        ident[end + 1] = '\0';
        word *temp_word = malloc(sizeof(word));
        if (check_if_keyword(ident)) {
          tail->type = KEYWORD;
        } else {
          tail->type = IDENTIFIER;
        }
        tail->value = ident;
        tail->next = temp_word;
        tail = temp_word;

        tail->type = OPERATOR;
        tail->value = "=";
      } break;
      case ':':
        break;
      case '+':
        break;
      case '[': {
        word *temp_word = malloc(sizeof(word));
        tail->next = temp_word;
        tail = tail->next;

        tail->type = PUNCTUATION;
        tail->p_type = OPEN_SQR_BR;
        tail->value = "[";
      } break;
      case ']': {
        word *temp_word = malloc(sizeof(word));
        tail->next = temp_word;
        tail = tail->next;

        tail->type = PUNCTUATION;
        tail->p_type = CLOSING_SQR_BR;
        tail->value = "]";
      } break;
      case '\"': {
        word *temp_word = malloc(sizeof(word));
        tail->next = temp_word;
        tail = tail->next;

        tail->p_type = QUOTE;
        tail->value = "\"";
      } break;
      default:
        break;
      }
    }
    if (tail == word_line) {
      line_words[i] = NULL;
      continue;
    }
    line_words[i] = word_line;
  }
  for (int i = 0; i < line_count; ++i) {
    word *wrd = line_words[i];
    while (wrd != NULL) {
      switch (wrd->type) {
      case PUNCTUATION:
        switch (wrd->p_type) {
        case CLOSING_SQR_BR:
          printf("closing sqr br %s\n", wrd->value);
          break;
        case OPEN_SQR_BR:
          printf("open sqr br %s\n", wrd->value);
          break;
        default:
          break;
        }
        break;
      case OPERATOR:
        switch (wrd->o_type) {
        case EQL_SIGN:
          printf("OPERATOR %s\n", wrd->value);
          break;
        case PLUS_SIGN:
          break;
        }
        break;
      case KEYWORD:
        printf("K\n");
        break;
      case IDENTIFIER:
        printf("I %s\n", wrd->value);
        break;
      default:
        break;
      }
      wrd = wrd->next;
    }
  }
  for (int i = 0; i < line_count; ++i)
    free(line_words[i]);
  free(line_words);
  for (int i = 0; i < line_count; ++i)
    free(lines[i]);
  free(lines);
}
