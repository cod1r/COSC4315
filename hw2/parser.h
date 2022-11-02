#ifndef PARSE_H
#define PARSE_H
#include "data_types.h"
#include "lexer.h"
#include <string>
#include <vector>
struct data_node {
  value_obj val;
};
std::vector<data_node> parse(std::vector<word>);
std::vector<data_node> create_data_node();
#endif
