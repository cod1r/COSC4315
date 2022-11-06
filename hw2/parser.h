#ifndef PARSE_H
#define PARSE_H
#include "data_types.h"
#include "lexer.h"
#include <string>
#include <vector>
#define NUMBER_TYPE (long long)
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
} ExprType;
typedef enum LitType {
  Num_t,
  Str_t,
} LitType;
struct Expression {
  ExprType expr_type;
  size_t start;
  size_t end;
  size_t indent_lvl;
};
struct Identifier : public Expression {
  std::string name;
};
struct FunctionDeclaration : public Expression {
	Identifier identifier;
	std::vector<Identifier> args;
	std::vector<Expression> body;
};
struct Condition : public Expression {
	std::vector<Expression> body;
};
struct If : public Expression {
	Condition condition;
	std::vector<Expression> body;
};
struct Literal : public Expression {
  LitType lit_type;
  void *value;
};
struct BinaryExpression : public Expression {
  OPERATOR_TYPE op_type;
  Expression Left;
  Expression Right;
};
struct Program : public Expression {
  std::vector<Expression> body;
};
Expression parse2(std::vector<word>);
#endif
