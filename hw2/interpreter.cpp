#include "interpreter.h"
#include "parser.h"
#include <functional>
#include <iostream>
#include <map>
#include <stack>
#include <vector>
data_node eval_rhs(std::vector<data_node> nodes,
                   std::map<std::string, data_node> &symbol_table) {
  data_node final_value;
  final_value.val.t = NO_TYPE;
  size_t node_idx = 0;
  while (node_idx < nodes.size()) {
    data_node current_node = nodes[node_idx];
    if (current_node.val.t == WORD) {
      word w = *(word *)current_node.val.obj;
      switch (w.type) {
      case PUNCTUATION: {
        switch (w.p_type) {
        case OPEN_SQR_BR:
          final_value.val.t = LIST;
          final_value.val.obj = new std::vector<long long>;
          ++node_idx;
          size_t node_idx_local = node_idx;
          for (; node_idx_local < nodes.size(); ++node_idx_local) {
            data_node temp_node = nodes[node_idx_local];
            if (temp_node.val.t == NUMBER) {
              (*(std::vector<long long> *)final_value.val.obj)
                  .push_back(*(long long *)temp_node.val.obj);
            } else if (temp_node.val.t == WORD) {
              word deciding_word = *(word *)temp_node.val.obj;
              if (deciding_word.type == PUNCTUATION) {
                if (deciding_word.p_type == CLOSING_SQR_BR) {
                  node_idx = node_idx_local + 1;
                  break;
                }
              }
            } else {
              throw std::runtime_error("type not implemented yet");
            }
          }
          if (node_idx != node_idx_local + 1) {
            throw std::runtime_error("unclosed sqr br");
          }
        }
        break;
      }
      case OPERATOR: {
        switch (w.o_type) {
        case PLUS_SIGN: {
          if (nodes.size() - 1 <= 0) {
            throw std::runtime_error("operator at end");
          }
          if (final_value.val.t == NO_TYPE) {
            throw std::runtime_error("operator at beginning");
          }
          data_node recurse_val = eval_rhs(
              std::vector<data_node>(nodes.begin() + node_idx + 1, nodes.end()),
              symbol_table);
          if (final_value.val.t == NUMBER and recurse_val.val.t == NUMBER) {
            *(long long *)final_value.val.obj +=
                *(long long *)recurse_val.val.obj;
          } else if (final_value.val.t == LIST and recurse_val.val.t == LIST) {
            std::vector<long long> *final_vec =
                (std::vector<long long> *)final_value.val.obj;
            final_vec->insert(
                final_vec->begin(),
                (*(std::vector<long long> *)recurse_val.val.obj).begin(),
                (*(std::vector<long long> *)recurse_val.val.obj).begin());
          } else {
            throw std::runtime_error("type mismatch");
          }
        } break;
        default:
          throw std::runtime_error("operator not implemented yet");
          break;
        }
      } break;
      case WHITESPACE:
        ++node_idx;
        break;
      }
    } else if (current_node.val.t == NUMBER) {
      final_value.val.t = NUMBER;
      final_value.val.obj = new long long;
      *(long long *)final_value.val.obj = *(long long *)current_node.val.obj;
      ++node_idx;
      for (size_t node_idx_local = node_idx; node_idx_local < nodes.size();
           ++node_idx_local) {
        data_node temp_node = nodes[node_idx_local];
        if (temp_node.val.t == WORD) {
          word w = *(word *)temp_node.val.obj;
          if (w.type == OPERATOR and w.o_type == PLUS_SIGN) {
            data_node r_val =
                eval_rhs(std::vector<data_node>(
                             nodes.begin() + node_idx_local + 1, nodes.end()),
                         symbol_table);
            *(long long *)final_value.val.obj += *(long long *)r_val.val.obj;
            return final_value;
          } else {
            throw std::runtime_error("unimplemented operator");
          }
        } else {
          throw std::runtime_error("unimplemented type in rhs");
        }
      }
    } else if (current_node.val.t == STRING) {
      final_value.val.t = STRING;
      final_value.val.obj = new std::string;
      *(std::string *)final_value.val.obj =
          *(std::string *)current_node.val.obj;
      break;
    } else if (current_node.val.t == IDENTIFIER_NODE) {
      if (symbol_table.find(*(std::string *)current_node.val.obj) ==
          symbol_table.end()) {
        throw std::runtime_error("undefined symbol");
      }
      final_value.val.t =
          symbol_table[*(std::string *)current_node.val.obj].val.t;
      if (final_value.val.t == NUMBER) {
        final_value.val.obj = new long long;
        *(long long *)final_value.val.obj =
            *(long long *)symbol_table[*(std::string *)current_node.val.obj]
                 .val.obj;
      } else if (final_value.val.t == LIST) {
        final_value.val.obj = new std::vector<long long>;
        *(std::vector<long long> *)final_value.val.obj =
            *(std::vector<long long> *)
                 symbol_table[*(std::string *)current_node.val.obj]
                     .val.obj;
      }
      ++node_idx;
      for (size_t node_idx_local = node_idx; node_idx_local < nodes.size();
           ++node_idx_local) {
        if (nodes[node_idx_local].val.t == WORD) {
          word temp_word = (*(word *)nodes[node_idx_local].val.obj);
          if (temp_word.type == OPERATOR and temp_word.o_type == PLUS_SIGN) {
            data_node r_val =
                eval_rhs(std::vector<data_node>(
                             nodes.begin() + node_idx_local + 1, nodes.end()),
                         symbol_table);
            if (final_value.val.t == NUMBER and r_val.val.t == NUMBER) {
              *(long long *)final_value.val.obj += *(long long *)r_val.val.obj;
            } else if (final_value.val.t == LIST and r_val.val.t == LIST) {
              std::vector<long long> *lst =
                  (std::vector<long long> *)final_value.val.obj;
              std::vector<long long> r_val_lst =
                  *(std::vector<long long> *)r_val.val.obj;
              lst->insert(lst->end(), r_val_lst.begin(), r_val_lst.end());
            } else {
              throw std::runtime_error("mismatched types");
            }
            return final_value;
          }
        }
      }
    }
  }
  return final_value;
}
template <typename T> bool check_true(T left, T right, OPERATOR_TYPE op) {
  if (op == EQUAL_BOOL) {
    return left == right;
  } else if (op == LESS_THAN_EQL_TO) {
    return left <= right;
  } else if (op == GREATER_THAN_EQL_TO) {
    return left >= right;
  } else if (op == GREATER_THAN) {
    return left > right;
  } else if (op == LESS_THAN) {
    return left < right;
  }
}
void run(std::vector<data_node> nodes) {
  std::map<std::string, data_node> symbol_table;
  size_t node_idx = 0;
  while (node_idx < nodes.size()) {
    data_node current_node = nodes[node_idx];
    switch (current_node.val.t) {
    case IDENTIFIER_NODE: {
      size_t unknown_idx = node_idx + 1;
      bool loop = true;
      while (loop) {
        if (nodes[unknown_idx].val.t == WORD) {
          word wrd = *(word *)nodes[unknown_idx].val.obj;
          switch (wrd.type) {
          case PUNCTUATION: {
            switch (wrd.p_type) {
            case OPEN_SQR_BR: {
              size_t index_count = 0;
              bool closing_sqr_br = false;
              size_t index = 0;
              ssize_t eql_idx = -1;
              ssize_t newline = -1;
              for (size_t check_idx = node_idx; check_idx < nodes.size();
                   ++check_idx) {
                if (nodes[check_idx].val.t == NUMBER and eql_idx == -1) {
                  ++index_count;
                  if (index_count > 1)
                    throw std::runtime_error("syntax number");
                  index = *(long long *)nodes[check_idx].val.obj;
                } else if (nodes[check_idx].val.t == WORD and
                           (*(word *)nodes[check_idx].val.obj).type ==
                               PUNCTUATION and
                           (*(word *)nodes[check_idx].val.obj).p_type ==
                               CLOSING_SQR_BR) {
                  closing_sqr_br = true;
                } else if (nodes[check_idx].val.t == WORD and
                           (*(word *)nodes[check_idx].val.obj).type ==
                               OPERATOR and
                           (*(word *)nodes[check_idx].val.obj).o_type ==
                               EQL_SIGN) {
                  eql_idx = check_idx;
                } else if (nodes[check_idx].val.t == WORD and
                           (*(word *)nodes[check_idx].val.obj).type ==
                               WHITESPACE and
                           (*(word *)nodes[check_idx].val.obj)
                                   .value.find("\n") != std::string::npos) {
                  newline = check_idx;
                  break;
                }
              }
              if (closing_sqr_br == false) {
                throw std::runtime_error("unclosed");
              }
              if (symbol_table.find(*(std::string *)current_node.val.obj) ==
                  symbol_table.end()) {
                throw std::runtime_error(
                    "cannot do subscript on undefined identifier");
              }
              std::vector<long long> *list_obj =
                  (std::vector<long long> *)
                      symbol_table[*(std::string *)current_node.val.obj]
                          .val.obj;
              data_node rhs =
                  eval_rhs(std::vector<data_node>(nodes.begin() + eql_idx + 1,
                                                  nodes.begin() + newline),
                           symbol_table);
              if (rhs.val.t == NUMBER) {
                (*list_obj)[index] = *(long long *)rhs.val.obj;
              }
              node_idx = newline + 1;
              loop = false;
            } break;
            case COLON:
              break;
            }
          } break;
          case OPERATOR:
            // wrd is the node 1 ahead of the current_node
            if (wrd.value == "=") {
              size_t find_newline_idx = unknown_idx + 1;
              while (find_newline_idx < nodes.size()) {
                if (nodes[find_newline_idx].val.t == WORD) {
                  word w = *(word *)nodes[find_newline_idx].val.obj;
                  if (w.type == WHITESPACE and
                      w.value.find("\n") != std::string::npos) {
                    break;
                  }
                }
                ++find_newline_idx;
              }
              data_node rhs = eval_rhs(
                  std::vector<data_node>(nodes.begin() + unknown_idx + 1,
                                         nodes.begin() + find_newline_idx),
                  symbol_table);
              symbol_table[*(std::string *)current_node.val.obj] = rhs;
              node_idx = find_newline_idx + 1;
              loop = false;
            }
            break;
          case WHITESPACE:
            ++unknown_idx;
            break;
          }
        }
      }
    } break;
    case KEYWORD_NODE: {
      std::string keyword = *(std::string *)current_node.val.obj;
      if (keyword == "print") {
        std::stack<data_node> confirm_stk;
        std::vector<data_node> print_values;
        size_t node_idx_local = node_idx;
        for (; node_idx_local < nodes.size(); ++node_idx_local) {
          if (nodes[node_idx_local].val.t == WORD and
              (*(word *)nodes[node_idx_local].val.obj).type == PUNCTUATION) {
            word temp_word = *(word *)nodes[node_idx_local].val.obj;
            if (temp_word.p_type == OPEN_PARENTH) {
              confirm_stk.push(nodes[node_idx_local]);
            } else if (temp_word.p_type == CLOSE_PARENTH and
                       confirm_stk.size() > 0 and
                       (*(word *)confirm_stk.top().val.obj).p_type ==
                           OPEN_PARENTH) {
              confirm_stk.pop();
            } else if (temp_word.p_type == COMMA) {
              data_node comma_so_space;
              comma_so_space.val.t = STRING;
              comma_so_space.val.obj = new std::string(3, ' ');
              print_values.push_back(comma_so_space);
            }
          } else if (nodes[node_idx_local].val.t == NUMBER or
                     nodes[node_idx_local].val.t == IDENTIFIER_NODE or
                     nodes[node_idx_local].val.t == STRING) {
            print_values.push_back(nodes[node_idx_local]);
          }
        }
        if (confirm_stk.size() != 0) {
          throw std::runtime_error("syntax error");
        }
        for (data_node prnt_value_node : print_values) {
          if (prnt_value_node.val.t == NUMBER) {
            std::cout << *(long long *)prnt_value_node.val.obj;
          } else if (prnt_value_node.val.t == IDENTIFIER_NODE) {
            if (symbol_table[*(std::string *)prnt_value_node.val.obj].val.t ==
                NUMBER) {
              std::cout
                  << *(long long *)
                          symbol_table[*(std::string *)prnt_value_node.val.obj]
                              .val.obj;
            } else if (symbol_table[*(std::string *)prnt_value_node.val.obj]
                           .val.t == LIST) {
              std::vector<long long> lst =
                  *(std::vector<long long> *)
                       symbol_table[*(std::string *)prnt_value_node.val.obj]
                           .val.obj;
              std::cout << "[";
              for (long long num : lst) {
                if (num != lst.back()) {
                  std::cout << num << ", ";
                } else {
                  std::cout << num;
                }
              }
              std::cout << "]";
            }
          } else if (prnt_value_node.val.t == STRING) {
            std::string str_val = *(std::string *)prnt_value_node.val.obj;
            std::cout << std::string(str_val.begin() + 1, str_val.end() - 1);
          }
        }
        std::cout << std::endl;
        node_idx = node_idx_local;
      } else if (keyword == "if") {
        // we need to be able to handle nested if statements as well.
        // else blocks included
        size_t if_idx = node_idx;
        size_t colon_idx = node_idx;
        while (colon_idx < nodes.size()) {
          if (nodes[colon_idx].val.t == WORD) {
            word curr_word = *(word *)nodes[colon_idx].val.obj;
            if (curr_word.type == PUNCTUATION and curr_word.p_type == COLON) {
              break;
            }
          }
          ++colon_idx;
        }
				if (colon_idx >= nodes.size()) {
					throw std::runtime_error("no colon found");
				}
        while (if_idx < nodes.size()) {
          if (nodes[if_idx].val.t == WORD) {
            word curr_word = *(word *)nodes[if_idx].val.obj;
            if (curr_word.type == OPERATOR) {

              size_t right_side_operand_idx = if_idx + 1;
              size_t left_side_operand_idx = if_idx + 1;
              while (left_side_operand_idx > node_idx) {
                if (nodes[left_side_operand_idx].val.t == IDENTIFIER_NODE or
                    nodes[left_side_operand_idx].val.t == NUMBER or
                    nodes[left_side_operand_idx].val.t == STRING) {
                  break;
                }
                --left_side_operand_idx;
              }
              while (right_side_operand_idx < colon_idx) {
                if (nodes[right_side_operand_idx].val.t == IDENTIFIER_NODE or
                    nodes[right_side_operand_idx].val.t == NUMBER or
                    nodes[left_side_operand_idx].val.t == STRING) {
                  break;
                }
                ++right_side_operand_idx;
              }
              bool is_true = false;
              if (nodes[left_side_operand_idx].val.t == IDENTIFIER_NODE or
                  nodes[right_side_operand_idx].val.t == IDENTIFIER_NODE) {
                if (nodes[left_side_operand_idx].val.t == IDENTIFIER_NODE and
                    nodes[right_side_operand_idx].val.t != IDENTIFIER_NODE) {
                  data_node left =
                      symbol_table[*(std::string *)nodes[left_side_operand_idx]
                                        .val.obj];
                  if (nodes[right_side_operand_idx].val.t == left.val.t) {
                    if (left.val.t == STRING) {
                      is_true = check_true<std::string>(
                          *(std::string *)left.val.obj,
                          *(std::string *)nodes[right_side_operand_idx].val.obj, curr_word.o_type);
                    } else if (left.val.t == NUMBER) {
                      is_true = check_true<NUM_TYPE>(
                          *(NUM_TYPE *)left.val.obj,
                          *(NUM_TYPE *)nodes[right_side_operand_idx].val.obj, curr_word.o_type);
                    }
                  }
                } else if (nodes[left_side_operand_idx].val.t !=
                               IDENTIFIER_NODE and
                           nodes[right_side_operand_idx].val.t ==
                               IDENTIFIER_NODE) {
                  data_node right =
                      symbol_table[*(std::string *)nodes[right_side_operand_idx]
                                        .val.obj];
                  if (nodes[left_side_operand_idx].val.t == right.val.t) {
                    if (right.val.t == STRING) {
                      is_true = check_true<std::string>(
                          *(std::string *)nodes[left_side_operand_idx].val.obj,
                          *(std::string *)right.val.obj, curr_word.o_type);
                    } else if (right.val.t == NUMBER) {
                      is_true = check_true<NUM_TYPE>(
                          *(NUM_TYPE *)nodes[left_side_operand_idx].val.obj,
                          *(NUM_TYPE *)right.val.obj, curr_word.o_type);
                    }
                  }

                } else {
                  throw std::runtime_error("shit");
                }
              } else if (nodes[left_side_operand_idx].val.t ==
                         nodes[right_side_operand_idx].val.t) {
                if (nodes[left_side_operand_idx].val.t == STRING) {
                  std::string left =
                      *(std::string *)nodes[left_side_operand_idx].val.obj;
                  std::string right =
                      *(std::string *)nodes[right_side_operand_idx].val.obj;

                  is_true =
                      check_true<std::string>(left, right, curr_word.o_type);
                } else if (nodes[left_side_operand_idx].val.t == NUMBER) {
                  NUM_TYPE left =
                      *(NUM_TYPE *)nodes[left_side_operand_idx].val.obj;
                  NUM_TYPE right =
                      *(NUM_TYPE *)nodes[right_side_operand_idx].val.obj;
                  is_true = check_true<NUM_TYPE>(left, right, curr_word.o_type);
                } else {
                  throw std::runtime_error(
                      "type not implemented yet for if conditions");
                }
              }
							if (is_true) {
								node_idx = colon_idx + 2;
								break;
							} else {
								if (nodes[node_idx - 1].val.t == WORD and
										(*(word*)nodes[node_idx - 1].val.obj).type == WHITESPACE) {
									size_t indent_level = (*(word*)nodes[node_idx - 1].val.obj).value.length();
									size_t else_block = if_idx + 1;
									while (else_block < nodes.size()) {
										if (nodes[else_block].val.t == KEYWORD_NODE) {
											if (*(std::string*)nodes[else_block].val.obj == "else") {
												if (nodes[else_block - 1].val.t == WORD and
														(*(word*)nodes[else_block - 1].val.obj).type == WHITESPACE and
														(*(word*)nodes[else_block - 1].val.obj).value.length() == indent_level) {
													break;
												}
											}
										}
										++else_block;
									}
									if (else_block < nodes.size()) {
										// plus 2 because of the colon
										node_idx = else_block + 2;
										break;
									} else {
										node_idx = else_block;
									}
								} else {
									throw std::runtime_error("not whitespace");
								}
							}
            }
          }
          ++if_idx;
        }
      } else if (keyword == "def") {
				throw std::runtime_error("def not implemented yet");
			} else if (keyword == "else") {
				// we need to know indent levels somehow
      } else {
        throw std::runtime_error("unimplemented keyword");
      }
    } break;
		case NUMBER:
			throw std::runtime_error(std::to_string((*(NUM_TYPE*)nodes[node_idx].val.obj)));
			break;
    case WORD:
      ++node_idx;
      break;
    default:
      std::cout << node_idx << " " << nodes.size() << " "  << current_node.val.t << std::endl;
      throw std::runtime_error("unimplemented node type");
      break;
    }
  }
}
