#ifndef DATA_TYPE_H
#define DATA_TYPE_H
#include <iostream>
#include <vector>
typedef enum type {
  NUMBER,
  STRING,
  BOOLEAN,
  LIST,
  IDENTIFIER_NODE,
  NO_TYPE,
  WORD,
  NONE,
  KEYWORD_NODE,
  FUNCTION,
} type;

struct value_obj {
  type t;
  void *obj;
};

struct function_obj {
  size_t start;
  size_t end;
  std::vector<std::string> args;
};
#endif
