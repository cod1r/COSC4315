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
  NEWLINE,
  INDENT,
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
  GREATER_THAN,
  LESS_THAN,
  NOT_EQUAL_TO,
  LESS_THAN_EQL_TO,
  GREATER_THAN_EQL_TO,
  EQUAL_BOOL,
} OPERATOR_TYPE;
struct word {
  WORD_TYPE type;
  std::string value;
  PUNCTUATION_TYPE p_type;
  OPERATOR_TYPE o_type;
};
static const char COMMENT_CHAR = '#';
static const std::vector<std::string> PUNCTUATIONS = {
    ":", "[", "]", "\"", "(", ")", ".", ",", "\'",
};
static const std::vector<std::string> OPERATORS = {
    "==", "!=", ">=", "<=", "//", "/", "+", "=", "*", "-", "<", ">",
};
static const std::vector<std::string> KEYWORDS = {
    "pass",  "def",    "if",    "elif", "else", "for",  "True",
    "False", "return", "print", "not",  "len",  "while"};
std::vector<word> lexer(char *file_name);
#endif
