#ifndef PARSE_H
#define PARSE_H
#include "data_types.h"
#include "lexer.h"
#include <string>
#include <vector>
#define NUM_TYPE long long
#define STR_TYPE std::string
#define OP_TYPE char
struct data_node {
  value_obj val;
};
std::vector<data_node> parse(std::vector<word>);

typedef enum ExprType {
  BinaryExpression_t,
  Assignment_t,
  Program_t,
  FunctionDeclaration_t,
  VariableDeclaration_t,
  FunctionCall_t,
  If_t,
  Conditional_t,
  List_t,
  WORD_UNKNOWN,
  ObjectAccess,
} ExprType;
typedef enum LitType {
  Num_t,
  Str_t,
  Op_t,
} LitType;
typedef enum AccessType {
  string_t,
  num_t,
  identifier_t,
  obj_access_t,
} AccessType;
struct Expression {
  ExprType expr_type = WORD_UNKNOWN;
  size_t start;
  size_t end;
  size_t indent_lvl;
};
struct WORD_EXPR : public Expression {
  ExprType expr_type;
  std::vector<word> words;
};
struct Identifier : public Expression {
  word id;
};
struct ObjectAccessExpr : public WORD_EXPR {
  Identifier identifier;
  AccessType access_t;
  void *accessor;
};
struct FunctionDeclaration : public WORD_EXPR {
  Identifier identifier;
  std::vector<Identifier> args;
  std::vector<Expression> body;
};
struct Literal : public WORD_EXPR {
  LitType lit_type;
};
struct Condition : public Expression {
  Literal lit;
  Condition *Left = nullptr;
  Condition *Right = nullptr;
};
struct If : public Expression {
  Condition condition;
  std::vector<Expression> body;
};
struct BinaryExpression : public Expression {
  Literal lit;
  BinaryExpression *Left = nullptr;
  BinaryExpression *Right = nullptr;
};
struct ListExpression : public Expression {
  std::vector<Expression> members;
};
struct Program : public Expression {
  std::vector<Expression> body;
};
Expression parse2(std::vector<word>);
bool try_binary_expr(std::vector<word>);
#endif
