#include "interpreter.h"
#include "parser.h"
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
      } else {
        throw std::runtime_error("unimplemented keyword");
      }
    } break;
    case WORD:
      ++node_idx;
      break;
    default:
      std::cout << current_node.val.t << std::endl;
      throw std::runtime_error("unimplemented node type");
      break;
    }
  }
}
