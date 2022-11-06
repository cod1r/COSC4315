#ifndef LEXER_H
#define LEXER_H
#include <string>
#include <vector>
typedef enum WORD_TYPE {
  IDENTIFIER,
  KEYWORD,
  PUNCTUATION,
  OPERATOR,
  STRING_LITERAL,
  NUMBER_LITERAL,
  WHITESPACE,
  COMMENT,
} WORD_TYPE;
typedef enum PUNCTUATION_TYPE {
  OPEN_SQR_BR,
  CLOSING_SQR_BR,
  OPEN_PARENTH,
  CLOSE_PARENTH,
  COLON,
  QUOTE,
  COMMA,
  PERIOD,
} PUNCTUATION_TYPE;
typedef enum OPERATOR_TYPE {
  EQL_SIGN,
  PLUS_SIGN,
  MULTIPLY,
  SUBTRACT,
  DIVIDE,
} OPERATOR_TYPE;
struct word {
  WORD_TYPE type;
  std::string value;
  PUNCTUATION_TYPE p_type;
  OPERATOR_TYPE o_type;
};
static const char COMMENT_CHAR = '#';
static const char PUNCTUATIONS[] = {
    ':', '[', ']', '\"', '(', ')', '.', ',', '\'',
};
static const char OPERATORS[] = {
    '+',
    '=',
    '*',
    '-',
};
static const char *KEYWORDS[] = {"pass", "def",  "if",    "elif",   "else",
                                 "for",  "True", "False", "return", "print"};
std::vector<word> lexer(char *file_name);
#endif
