#include "interpreter.h"
#include "parser.h"
#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <stack>
#include <vector>
data_node eval_rhs(std::vector<data_node> nodes,
                   std::map<std::string, data_node> &symbol_table,
                   bool in_function, std::string function_name) {
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
        case OPEN_SQR_BR: {
          final_value.val.t = LIST;
          final_value.val.obj = new std::vector<NUM_TYPE>;
          ++node_idx;
          size_t node_idx_local = node_idx;
          for (; node_idx_local < nodes.size(); ++node_idx_local) {
            data_node temp_node = nodes[node_idx_local];
            if (temp_node.val.t == NUMBER) {
              (*(std::vector<NUM_TYPE> *)final_value.val.obj)
                  .push_back(*(NUM_TYPE *)temp_node.val.obj);
            } else if (temp_node.val.t == WORD) {
              word deciding_word = *(word *)temp_node.val.obj;
              if (deciding_word.type == PUNCTUATION) {
                if (deciding_word.p_type == CLOSING_SQR_BR) {
                  node_idx = node_idx_local + 1;
                  break;
                }
              }
						} else if (temp_node.val.t == IDENTIFIER_NODE) {
              (*(std::vector<NUM_TYPE> *)final_value.val.obj)
                  .push_back(*(NUM_TYPE *)symbol_table[*(std::string*)temp_node.val.obj].val.obj);
            } else {
              throw std::runtime_error("type not implemented yet");
            }
          }
          if (node_idx != node_idx_local + 1) {
            throw std::runtime_error("unclosed sqr br");
          }
        } break;
        default:
          throw std::runtime_error("unimplemented punctuation: " +
                                   std::to_string(w.p_type));
          break;
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
              symbol_table, in_function, function_name);
          if (final_value.val.t == NUMBER and recurse_val.val.t == NUMBER) {
            *(NUM_TYPE *)final_value.val.obj +=
                *(NUM_TYPE *)recurse_val.val.obj;
          } else if (final_value.val.t == LIST and recurse_val.val.t == LIST) {
            std::vector<NUM_TYPE> *final_vec =
                (std::vector<NUM_TYPE> *)final_value.val.obj;
            final_vec->insert(
                final_vec->begin(),
                (*(std::vector<NUM_TYPE> *)recurse_val.val.obj).begin(),
                (*(std::vector<NUM_TYPE> *)recurse_val.val.obj).begin());
          } else {
            throw std::runtime_error("type mismatch");
          }
        } break;
        default:
          throw std::runtime_error("operator not implemented yet");
          break;
        }
      } break;
      case INDENT:
      case NEWLINE:
        ++node_idx;
        break;
      }
    } else if (current_node.val.t == NUMBER) {
      final_value.val.t = NUMBER;
      final_value.val.obj = new NUM_TYPE;
      *(NUM_TYPE *)final_value.val.obj = *(NUM_TYPE *)current_node.val.obj;
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
                         symbol_table, in_function, function_name);
            *(NUM_TYPE *)final_value.val.obj += *(NUM_TYPE *)r_val.val.obj;
            return final_value;
          } else if (w.type == OPERATOR) {
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
      if (nodes[node_idx + 1].val.t == WORD and
          (*(word *)nodes[node_idx + 1].val.obj).type == PUNCTUATION and
          (*(word *)nodes[node_idx + 1].val.obj).p_type == OPEN_SQR_BR) {
        data_node look_up = symbol_table[*(std::string *)current_node.val.obj];
        ssize_t colon = -1;
        for (size_t colon_idx = node_idx; colon_idx < nodes.size();
             ++colon_idx) {
          if (nodes[colon_idx].val.t == WORD and
              (*(word *)nodes[colon_idx].val.obj).type == PUNCTUATION and
              (*(word *)nodes[colon_idx].val.obj).p_type == COLON) {
            colon = colon_idx;
            break;
          }
        }
        size_t clsing = node_idx + 1;
        while (clsing < nodes.size()) {
          if (nodes[clsing].val.t == WORD and
              (*(word *)nodes[clsing].val.obj).type == PUNCTUATION and
              (*(word *)nodes[clsing].val.obj).p_type == CLOSING_SQR_BR) {
            break;
          }
          ++clsing;
        }
        if (colon != -1) {
          size_t beg_idx = node_idx + 1;
          while (beg_idx < colon) {
            if (nodes[beg_idx].val.t == IDENTIFIER_NODE or
                nodes[beg_idx].val.t == NUMBER) {
              break;
            }
            ++beg_idx;
          }
          size_t end_idx = clsing;
          while (end_idx > colon) {
            if (nodes[end_idx].val.t == IDENTIFIER_NODE or
                nodes[end_idx].val.t == NUMBER) {
              break;
            }
            --end_idx;
          }
          if (nodes[beg_idx].val.t == IDENTIFIER_NODE or
              nodes[end_idx].val.t == IDENTIFIER_NODE) {
            throw std::runtime_error(
                "did not write code for if things were identifiers");
          }
          size_t beg =
              (beg_idx < colon ? *(NUM_TYPE *)nodes[beg_idx].val.obj : 0);
          size_t end =
              (end_idx > colon
                   ? *(NUM_TYPE *)nodes[end_idx].val.obj
                   : (*(std::vector<NUM_TYPE> *)look_up.val.obj).size());
          final_value.val.t = LIST;
          final_value.val.obj = new std::vector<NUM_TYPE>;
          for (size_t new_lst_idx = beg; new_lst_idx < end; ++new_lst_idx) {
            (*(std::vector<NUM_TYPE> *)final_value.val.obj)
                .push_back((*(std::vector<NUM_TYPE> *)look_up.val.obj)
                               .at(new_lst_idx));
          }
          size_t find_plus = node_idx;
          while (find_plus < nodes.size()) {
            if (nodes[find_plus].val.t == WORD and
                (*(word *)nodes[find_plus].val.obj).type == OPERATOR and
                (*(word *)nodes[find_plus].val.obj).o_type == PLUS_SIGN) {
              data_node rrhs =
                  eval_rhs(std::vector<data_node>(nodes.begin() + find_plus + 1,
                                                  nodes.end()),
                           symbol_table, in_function, function_name);
              ;
              if (final_value.val.t == NUMBER and rrhs.val.t == NUMBER) {
                *(NUM_TYPE *)final_value.val.obj += *(NUM_TYPE *)rrhs.val.obj;
              } else if (final_value.val.t == LIST and rrhs.val.t == LIST) {
                std::vector<NUM_TYPE> *lst =
                    (std::vector<NUM_TYPE> *)final_value.val.obj;
                std::vector<NUM_TYPE> rrhs_lst =
                    *(std::vector<NUM_TYPE> *)rrhs.val.obj;
                lst->insert(lst->end(), rrhs_lst.begin(), rrhs_lst.end());
              } else {
                throw std::runtime_error("mismatched types");
              }
              break;
            }
            ++find_plus;
          }
          return final_value;
        } else {
          size_t index = 0;
          size_t index_idx = node_idx + 2;
          while (index_idx < nodes.size()) {
            if (nodes[index_idx].val.t == IDENTIFIER_NODE or
                nodes[index_idx].val.t == NUMBER) {
              if (nodes[index_idx].val.t == IDENTIFIER_NODE) {
                if (symbol_table[*(std::string *)nodes[index_idx].val.obj]
                        .val.t != NUMBER) {
                  std::cout
                      << symbol_table[*(std::string *)nodes[index_idx].val.obj]
                             .val.t
                      << std::endl;
                  throw std::runtime_error("index not number");
                }
                index =
                    *(NUM_TYPE *)
                         symbol_table[*(std::string *)nodes[index_idx].val.obj + (in_function ? "_" + function_name: "")]
                             .val.obj;
              } else {
                index = *(NUM_TYPE *)nodes[index_idx].val.obj;
              }
              break;
            }
            ++index_idx;
          }
          final_value.val.t = NUMBER;
          final_value.val.obj = new NUM_TYPE;
          *(NUM_TYPE *)final_value.val.obj =
              (*(std::vector<NUM_TYPE> *)look_up.val.obj)[index];
          size_t find_plus = node_idx;
          while (find_plus < nodes.size()) {
            if (nodes[find_plus].val.t == WORD and
                (*(word *)nodes[find_plus].val.obj).type == OPERATOR and
                (*(word *)nodes[find_plus].val.obj).o_type == PLUS_SIGN) {
              data_node rrhs =
                  eval_rhs(std::vector<data_node>(nodes.begin() + find_plus + 1,
                                                  nodes.end()),
                           symbol_table, in_function, function_name);
              ;
              if (final_value.val.t == NUMBER and rrhs.val.t == NUMBER) {
                *(NUM_TYPE *)final_value.val.obj += *(NUM_TYPE *)rrhs.val.obj;
              } else if (final_value.val.t == LIST and rrhs.val.t == LIST) {
                std::vector<NUM_TYPE> *lst =
                    (std::vector<NUM_TYPE> *)final_value.val.obj;
                std::vector<NUM_TYPE> rrhs_lst =
                    *(std::vector<NUM_TYPE> *)rrhs.val.obj;
                lst->insert(lst->end(), rrhs_lst.begin(), rrhs_lst.end());
              } else {
                throw std::runtime_error("mismatched types");
              }
              break;
            }
            ++find_plus;
          }
        }
        return final_value;
      } else {
        if ((symbol_table.find(*(std::string *)current_node.val.obj) ==
             symbol_table.end()) and
            (in_function and
             symbol_table.find(*(std::string *)current_node.val.obj + "_" +
                               function_name) == symbol_table.end())) {
          throw std::runtime_error("undefined symbol");
        }
        std::string identifier_name = *(std::string *)current_node.val.obj +
                                      (in_function ? "_" + function_name : "");
        final_value.val.t = symbol_table[identifier_name].val.t;
        if (final_value.val.t == NUMBER) {
          final_value.val.obj = new NUM_TYPE;
          assert(symbol_table.find(identifier_name) != symbol_table.end());
          assert(final_value.val.obj != nullptr);
          assert(symbol_table[identifier_name].val.obj != nullptr);
          *(NUM_TYPE *)final_value.val.obj =
              *(NUM_TYPE *)symbol_table[identifier_name].val.obj;
        } else if (final_value.val.t == LIST) {
          final_value.val.obj = new std::vector<NUM_TYPE>;
          *(std::vector<NUM_TYPE> *)final_value.val.obj =
              *(std::vector<NUM_TYPE> *)symbol_table[identifier_name].val.obj;
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
                           symbol_table, in_function, function_name);
              if (final_value.val.t == NUMBER and r_val.val.t == NUMBER) {
                *(NUM_TYPE *)final_value.val.obj += *(NUM_TYPE *)r_val.val.obj;
              } else if (final_value.val.t == LIST and r_val.val.t == LIST) {
                std::vector<NUM_TYPE> *lst =
                    (std::vector<NUM_TYPE> *)final_value.val.obj;
                std::vector<NUM_TYPE> r_val_lst =
                    *(std::vector<NUM_TYPE> *)r_val.val.obj;
                lst->insert(lst->end(), r_val_lst.begin(), r_val_lst.end());
              } else if (final_value.val.t == IDENTIFIER_NODE or
                         r_val.val.t == IDENTIFIER_NODE) {
                throw std::runtime_error(
                    "cannot have returned value be an identifier type");
              } else {
                throw std::runtime_error("mismatched types");
              }
              return final_value;
            }
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
  size_t return_point = 0;
  bool in_function = false;
  std::string function_name;
  size_t node_idx = 0;
  std::string return_identifier_to;
  while (node_idx < nodes.size()) {
    data_node current_node = nodes[node_idx];
    switch (current_node.val.t) {
    case IDENTIFIER_NODE: {
      size_t unknown_idx = node_idx + 1;
      bool loop = true;
      while (loop) {
        if (unknown_idx < nodes.size() and nodes[unknown_idx].val.t == WORD) {
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
                  index = *(NUM_TYPE *)nodes[check_idx].val.obj;
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
                               NEWLINE) {
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
              ssize_t clsing = -1;
							for (size_t clsing_idx = unknown_idx + 1; clsing < newline; ++clsing_idx) {
                if (nodes[clsing_idx].val.t == WORD and
                    (*(word *)nodes[clsing_idx].val.obj).type == PUNCTUATION and
                    (*(word *)nodes[clsing_idx].val.obj).p_type == CLOSING_SQR_BR) {
                  clsing = clsing_idx;
                  break;
                }
							}
              ssize_t colon = -1;
              for (size_t colon_idx = unknown_idx + 1; colon_idx < clsing;
                   ++colon_idx) {
                if (nodes[colon_idx].val.t == WORD and
                    (*(word *)nodes[colon_idx].val.obj).type == PUNCTUATION and
                    (*(word *)nodes[colon_idx].val.obj).p_type == COLON) {
                  colon = colon_idx;
                  break;
                }
              }
              std::vector<NUM_TYPE> *list_obj =
                  (std::vector<NUM_TYPE> *)
                      symbol_table[*(std::string *)current_node.val.obj]
                          .val.obj;
              data_node rhs =
                  eval_rhs(std::vector<data_node>(nodes.begin() + eql_idx + 1,
                                                  nodes.begin() + newline),
                           symbol_table, in_function, function_name);
              if (colon != -1) {
                if (rhs.val.t != LIST) {
                  throw std::runtime_error("can only assign list to list");
                }
                size_t beg = unknown_idx;
                while (beg < colon) {
                  if (nodes[beg].val.t == IDENTIFIER_NODE or
                      nodes[beg].val.t == NUMBER) {
                    break;
                  }
                  ++beg;
                }
                size_t end = clsing;
                while (end > colon) {
                  if (nodes[end].val.t == IDENTIFIER_NODE or
                      nodes[end].val.t == NUMBER) {
                    break;
                  }
                  --end;
                }
                if (nodes[beg].val.t == IDENTIFIER_NODE or
                    nodes[end].val.t == IDENTIFIER_NODE) {
                  throw std::runtime_error(
                      "did not write code for if things were identifiers");
                }
                size_t beg_num =
                    (beg != colon ? *(NUM_TYPE *)nodes[beg].val.obj : 0);
                size_t end_num = (end != colon ? *(NUM_TYPE *)nodes[end].val.obj
                                               : list_obj->size());
                list_obj->erase(list_obj->begin() + beg_num,
                                list_obj->begin() + end_num);
                list_obj->insert(
                    list_obj->begin() + beg_num,
                    (*(std::vector<NUM_TYPE> *)rhs.val.obj).begin(),
                    (*(std::vector<NUM_TYPE> *)rhs.val.obj).end());
              } else {
                if (rhs.val.t == NUMBER) {
                  (*list_obj)[index] = *(NUM_TYPE *)rhs.val.obj;
                }
              }
              node_idx = newline + 1;
              loop = false;
            } break;
            case COLON:
															throw std::runtime_error("colon");
              break;
            case OPEN_PARENTH: {
              if (symbol_table[*(std::string *)current_node.val.obj].val.t !=
                  FUNCTION) {
                throw std::runtime_error(
                    "invalid syntax. identifier is not a function type");
              }
              node_idx =
                  (*(function_obj *)
                        symbol_table[*(std::string *)current_node.val.obj]
                            .val.obj)
                      .start;
              in_function = true;
              function_name = *(std::string *)current_node.val.obj;
              std::vector<data_node> initial_vals;
              size_t initial_args_idx = unknown_idx + 1;
              while (initial_args_idx < nodes.size()) {
                if (nodes[initial_args_idx].val.t == WORD and
                    (*(word *)nodes[initial_args_idx].val.obj).type ==
                        PUNCTUATION and
                    (*(word *)nodes[initial_args_idx].val.obj).p_type ==
                        CLOSE_PARENTH) {
                  break;
                }
                if (nodes[initial_args_idx].val.t == IDENTIFIER_NODE or
                    nodes[initial_args_idx].val.t == STRING or
                    nodes[initial_args_idx].val.t == NUMBER) {
                  data_node temp_node;
                  if (nodes[initial_args_idx].val.t == IDENTIFIER_NODE) {
                    assert(
                        symbol_table.find(
                            *(std::string *)nodes[initial_args_idx].val.obj) !=
                        symbol_table.end());
                    temp_node.val.t = IDENTIFIER_NODE;
                  } else {
                    temp_node.val.t = nodes[initial_args_idx].val.t;
                  }
                  temp_node.val.obj = nodes[initial_args_idx].val.obj;
                  initial_vals.push_back(temp_node);
                }
                ++initial_args_idx;
              }
              function_obj fn_obj =
                  (*(function_obj *)
                        symbol_table[*(std::string *)current_node.val.obj]
                            .val.obj);
              assert(initial_vals.size() == fn_obj.args.size());
              assert(function_name.length() > 0);
              for (size_t arg_idx = 0; arg_idx < fn_obj.args.size();
                   ++arg_idx) {
                if (initial_vals[arg_idx].val.t == IDENTIFIER_NODE) {
                  symbol_table[fn_obj.args[arg_idx]] =
                      symbol_table[*(std::string *)initial_vals[arg_idx]
                                        .val.obj];
                } else {
                  symbol_table[fn_obj.args[arg_idx]] = initial_vals[arg_idx];
                }
              }
              loop = false;
            } break;
            default:
              throw std::runtime_error("unimplemented punct.");
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
                  if (w.type == NEWLINE) {
                    break;
                  }
                }
                ++find_newline_idx;
              }
              size_t check_if_fun = unknown_idx + 1;
              while (check_if_fun < nodes.size() and !in_function) {
                if (nodes[check_if_fun].val.t == WORD and
                    (*(word *)nodes[check_if_fun].val.obj).type == NEWLINE) {
                  break;
                }
                if (nodes[check_if_fun].val.t == IDENTIFIER_NODE) {
                  if (symbol_table.find(
                          *(std::string *)nodes[check_if_fun].val.obj) !=
                          symbol_table.end() and
                      symbol_table[*(std::string *)nodes[check_if_fun].val.obj]
                              .val.t == FUNCTION) {
                    in_function = true;
                    function_name = *(std::string *)nodes[check_if_fun].val.obj;
                    std::vector<data_node> initial_vals;
                    size_t initial_args_idx = check_if_fun + 2;
                    while (initial_args_idx < nodes.size()) {
                      if (nodes[initial_args_idx].val.t == WORD and
                          (*(word *)nodes[initial_args_idx].val.obj).type ==
                              PUNCTUATION and
                          (*(word *)nodes[initial_args_idx].val.obj).p_type ==
                              CLOSE_PARENTH) {
                        break;
                      }
                      if (nodes[initial_args_idx].val.t == IDENTIFIER_NODE or
                          nodes[initial_args_idx].val.t == STRING or
                          nodes[initial_args_idx].val.t == NUMBER) {
                        data_node temp_node;
                        if (nodes[initial_args_idx].val.t == IDENTIFIER_NODE) {
                          assert(symbol_table.find(
                                     *(std::string *)nodes[initial_args_idx]
                                          .val.obj) != symbol_table.end());
                          temp_node.val.t = IDENTIFIER_NODE;
                        } else {
                          temp_node.val.t = nodes[initial_args_idx].val.t;
                        }
                        temp_node.val.obj = nodes[initial_args_idx].val.obj;
                        initial_vals.push_back(temp_node);
                      }
                      ++initial_args_idx;
                    }
                    function_obj fn_obj =
                        (*(function_obj *)symbol_table
                              [*(std::string *)nodes[check_if_fun].val.obj]
                                  .val.obj);
                    assert(initial_vals.size() == fn_obj.args.size());
                    assert(function_name.length() > 0);
                    for (size_t arg_idx = 0; arg_idx < fn_obj.args.size();
                         ++arg_idx) {
                      if (initial_vals[arg_idx].val.t == IDENTIFIER_NODE) {
                        symbol_table[fn_obj.args[arg_idx]] =
                            symbol_table[*(std::string *)initial_vals[arg_idx]
                                              .val.obj];
                      } else {
                        symbol_table[fn_obj.args[arg_idx]] =
                            initial_vals[arg_idx];
                      }
                    }
                    node_idx =
                        (*(function_obj *)symbol_table
                              [*(std::string *)nodes[check_if_fun].val.obj]
                                  .val.obj)
                            .start;
                    return_point = initial_args_idx + 1;
                    return_identifier_to = *(std::string *)current_node.val.obj;
                    goto skip;
                  }
                }
                ++check_if_fun;
              }
              {
                data_node rhs = eval_rhs(
                    std::vector<data_node>(nodes.begin() + unknown_idx + 1,
                                           nodes.begin() + find_newline_idx),
                    symbol_table, in_function, function_name);
                if (in_function) {
                  symbol_table[*(std::string *)current_node.val.obj + "_" +
                               function_name] = rhs;
                } else {
                  symbol_table[*(std::string *)current_node.val.obj] = rhs;
                }
                node_idx = find_newline_idx + 1;
              }
            skip : {}
              loop = false;
            }
            break;
          case INDENT:
					case NEWLINE:
						//std::cout << *(std::string*)current_node.val.obj << " " << wrd.type << std::endl;
						//throw std::runtime_error("whitespace");
						++unknown_idx;
						break;
					default: throw std::runtime_error("unimplemented word type: " + std::to_string(wrd.type)); break;
          }
        } else {
					break;
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
              (*(word *)nodes[node_idx_local].val.obj).type == NEWLINE) {
            ++node_idx_local;
            break;
          }
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
                     nodes[node_idx_local].val.t == STRING or
                     nodes[node_idx_local].val.t == LIST) {
            print_values.push_back(nodes[node_idx_local]);
          }
        }
        if (confirm_stk.size() != 0) {
          throw std::runtime_error("syntax error");
        }
        for (data_node prnt_value_node : print_values) {
          if (prnt_value_node.val.t == NUMBER) {
            std::cout << *(NUM_TYPE *)prnt_value_node.val.obj;
          } else if (prnt_value_node.val.t == IDENTIFIER_NODE) {
            if (symbol_table[*(std::string *)prnt_value_node.val.obj].val.t ==
                NUMBER) {
              std::cout
                  << *(NUM_TYPE *)
                          symbol_table[*(std::string *)prnt_value_node.val.obj]
                              .val.obj;
            } else if (symbol_table[*(std::string *)prnt_value_node.val.obj]
                           .val.t == LIST) {
              std::vector<NUM_TYPE> lst =
                  *(std::vector<NUM_TYPE> *)
                       symbol_table[*(std::string *)prnt_value_node.val.obj]
                           .val.obj;
              std::cout << "[";
              for (size_t idx = 0; idx < lst.size(); ++idx) {
                NUM_TYPE num = lst.at(idx);
                if (idx != lst.size() - 1) {
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
              size_t left_side_operand_idx = node_idx + 1;
              while (left_side_operand_idx < if_idx) {
                if (nodes[left_side_operand_idx].val.t == IDENTIFIER_NODE or
                    nodes[left_side_operand_idx].val.t == NUMBER or
                    nodes[left_side_operand_idx].val.t == STRING) {
                  break;
                }
                ++left_side_operand_idx;
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
                  if (left.val.t == LIST) {
                    size_t index_idx = left_side_operand_idx;
                    while (nodes[index_idx].val.t != NUMBER) {
                      ++index_idx;
                    }
                    is_true = check_true<NUM_TYPE>(
                        (*(std::vector<NUM_TYPE> *)left.val.obj)
                            .at(*(NUM_TYPE *)nodes[index_idx].val.obj),
                        *(NUM_TYPE *)nodes[right_side_operand_idx].val.obj,
                        curr_word.o_type);
                  } else if (nodes[right_side_operand_idx].val.t ==
                             left.val.t) {
                    if (left.val.t == STRING) {
                      is_true = check_true<std::string>(
                          *(std::string *)left.val.obj,
                          *(std::string *)nodes[right_side_operand_idx].val.obj,
                          curr_word.o_type);
                    } else if (left.val.t == NUMBER) {
                      is_true = check_true<NUM_TYPE>(
                          *(NUM_TYPE *)left.val.obj,
                          *(NUM_TYPE *)nodes[right_side_operand_idx].val.obj,
                          curr_word.o_type);
                    } else {
                      throw std::runtime_error("unimplemented type comparison");
                    }
                  } else {
                    throw std::runtime_error("not the same types");
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
                size_t indent_level = 0;
                if (nodes[node_idx - 1].val.t == WORD and
                    (*(word *)nodes[node_idx - 1].val.obj).type == INDENT) {
                  indent_level =
                      (*(word *)nodes[node_idx - 1].val.obj).value.length();
                }
                size_t else_block = if_idx + 1;
                while (else_block < nodes.size()) {
                  if (nodes[else_block].val.t == KEYWORD_NODE) {
                    if (*(std::string *)nodes[else_block].val.obj == "else") {
                      size_t compare_indent_level = 0;
                      if (nodes[else_block - 1].val.t == WORD and
                          (*(word *)nodes[else_block - 1].val.obj).type ==
                              INDENT) {
                        compare_indent_level =
                            (*(word *)nodes[else_block - 1].val.obj)
                                .value.length();
                      }
                      if (compare_indent_level == indent_level) {
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
              }
            }
          }
          ++if_idx;
        }
      } else if (keyword == "def") {
        data_node fun_node;
        fun_node.val.t = FUNCTION;
        fun_node.val.obj = new function_obj;
        std::string function_name;

        size_t name = node_idx;
        while (name < nodes.size()) {
          if (nodes[name].val.t == IDENTIFIER_NODE) {
            function_name = *(std::string *)nodes[name].val.obj;
            break;
          }
          ++name;
        }

        size_t args_idx = node_idx;
        {
          while (args_idx < nodes.size()) {
            if (nodes[args_idx].val.t == WORD and
                (*(word *)nodes[args_idx].val.obj).type == PUNCTUATION and
                (*(word *)nodes[args_idx].val.obj).p_type == OPEN_PARENTH) {
              while (args_idx < nodes.size()) {
                if (nodes[args_idx].val.t == WORD and
                    (*(word *)nodes[args_idx].val.obj).type == PUNCTUATION and
                    (*(word *)nodes[args_idx].val.obj).p_type == COLON) {
                  goto exit_loop;
                }
                if (nodes[args_idx].val.t == IDENTIFIER_NODE) {
                  (*(function_obj *)fun_node.val.obj)
                      .args.push_back(*(std::string *)nodes[args_idx].val.obj +
                                      "_" + function_name);
                }
                ++args_idx;
              }
            }
            ++args_idx;
          }
        exit_loop : {}
        }
        size_t function_idx = args_idx;
        {
          while (function_idx < nodes.size()) {
            if (nodes[function_idx].val.t == WORD and
                (*(word *)nodes[function_idx].val.obj).type == INDENT) {
              ++function_idx;
              break;
            }
            ++function_idx;
          }
          (*(function_obj *)fun_node.val.obj).start = function_idx;
        }

        {
          while (function_idx < nodes.size()) {
            if (nodes[function_idx].val.t == WORD and
                (*(word *)nodes[function_idx].val.obj).type == NEWLINE and
                function_idx + 1 < nodes.size() and
                nodes[function_idx + 1].val.t == WORD and
                (*(word *)nodes[function_idx + 1].val.obj).type != INDENT) {
              break;
            }
            if (nodes[function_idx].val.t == KEYWORD_NODE and
                *(std::string *)nodes[function_idx].val.obj == "return") {
              while (
                  nodes[function_idx].val.t != IDENTIFIER_NODE or
                  (nodes[function_idx].val.t == WORD and
                   (*(word *)nodes[function_idx].val.obj).type == NEWLINE and
                   function_idx + 1 < nodes.size() and
                   nodes[function_idx + 1].val.t == WORD and
                   (*(word *)nodes[function_idx + 1].val.obj).type != INDENT)) {
                ++function_idx;
              }
              break;
            }
            ++function_idx;
          }
          (*(function_obj *)fun_node.val.obj).end = function_idx;
        }

        if (nodes[name].val.t != IDENTIFIER_NODE) {
          throw std::runtime_error("invalid syntax for function1");
        }

        symbol_table[function_name] = fun_node;
        node_idx = function_idx + 1;
      } else if (keyword == "else") {
        size_t indent_lvl = 0;
        if (nodes[node_idx - 1].val.t == WORD and
            (*(word *)nodes[node_idx - 1].val.obj).type == INDENT) {
          indent_lvl = (*(word *)nodes[node_idx - 1].val.obj).value.length();
        }
        size_t else_end = node_idx + 1;
        while (else_end < nodes.size()) {
          size_t compare_indent_lvl = 0;
          if (nodes[else_end - 1].val.t == WORD and
              (*(word *)nodes[else_end - 1].val.obj).type == INDENT) {
            compare_indent_lvl =
                (*(word *)nodes[else_end - 1].val.obj).value.length();
          }
          if (compare_indent_lvl == indent_lvl and
              (*(word *)nodes[else_end].val.obj).type != NEWLINE and
              (*(word *)nodes[else_end].val.obj).type != INDENT and
              (*(word *)nodes[else_end].val.obj).type != PUNCTUATION and
              ((nodes[else_end - 1].val.t == WORD and
                (*(word *)nodes[else_end - 1].val.obj).type == INDENT) or
               (nodes[else_end - 1].val.t == WORD and
                (*(word *)nodes[else_end - 1].val.obj).type == NEWLINE))) {
            break;
          }
          ++else_end;
        }
        node_idx = else_end;
      } else if (keyword == "return") {
        in_function = false;
        size_t value_idx = node_idx + 1;
        while (value_idx < nodes.size()) {
          if (nodes[value_idx].val.t == IDENTIFIER_NODE or
              nodes[value_idx].val.t == STRING or
              nodes[value_idx].val.t == NUMBER) {
            break;
          }
          ++value_idx;
        }
        if (nodes[value_idx].val.t != IDENTIFIER_NODE and
            nodes[value_idx].val.t != STRING and
            nodes[value_idx].val.t != NUMBER) {
          throw std::runtime_error("syntax error: expected return value");
        }
        symbol_table[return_identifier_to] =
            symbol_table[*(std::string *)nodes[value_idx].val.obj + "_" +
                         function_name];
        node_idx = return_point;
      } else {
        throw std::runtime_error("unimplemented keyword");
      }
    } break;
    case NUMBER:
      ++node_idx;
      break;
    case WORD:
      ++node_idx;
      break;
    default:
      throw std::runtime_error("unimplemented node type");
      break;
    }
  }
}
