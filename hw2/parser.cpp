#include "parser.h"
#include "lexer.h"
#include <assert.h>
#include <cmath>
#include <iostream>
#include <map>
#include <stack>
#include <string>
#include <vector>

NUM_TYPE parse_num_literal(word w) {
  assert(w.type == NUMBER_LITERAL);
  NUM_TYPE num = 0;
  for (size_t idx = 0; idx < w.value.length(); ++idx) {
    num += (w.value[idx] - '0') * std::pow(10, w.value.length() - 1 - idx);
  }
  return num;
}

Expression construct_binary_expr(std::vector<word> words) {}

bool try_binary_expr(std::vector<word> words, size_t *word_idx) {
  size_t initial_word_idx = *word_idx;
  for (size_t binary_expr_word_idx = 0; binary_expr_word_idx < words.size();
       ++binary_expr_word_idx) {
    word current_word = words[binary_expr_word_idx];
    if (current_word.type == WHITESPACE and
        current_word.value.find('\n') != std::string::npos) {
      *word_idx = binary_expr_word_idx;
      break;
    } else if (current_word.type == WHITESPACE)
      continue;
    if (current_word.type != NUMBER_LITERAL and
        current_word.type != OPERATOR and current_word.type != PUNCTUATION) {
      return false;
    } else if (current_word.type == PUNCTUATION and
               (current_word.p_type != OPEN_PARENTH and
                current_word.p_type != CLOSE_PARENTH)) {
      return false;
    }
  }
  std::stack<word> stk;
  // checking if parentheses are balanced
  for (auto it = words.begin() + initial_word_idx; it < words.end(); ++it) {
    word w = *it;
    if (w.type == PUNCTUATION) {
      if (w.p_type == OPEN_PARENTH) {
        stk.push(w);
      } else if (stk.size() > 0 and w.p_type == CLOSE_PARENTH) {
        stk.pop();
      } else {
        throw std::runtime_error("unbalanced parentheses");
      }
    }
  }
  return true;
}

size_t try_assignment_expr(size_t starting, std::vector<word> words) {}

size_t try_element_access(size_t starting, std::vector<word> words) {}

size_t try_list_expr(size_t starting, std::vector<word> words) {}

Expression parse2(std::vector<word> &words) {
  size_t words_idx = 0;
  while (words_idx < words.size()) {
    word w = words[words_idx];
    if (w.type == PUNCTUATION and w.p_type == OPEN_SQR_BR) {
    } else if (w.type == KEYWORD) {
      if (w.value == "if") {
        // loop until you see something that is allowed to be in a condition
        // i.e parentheses or conditional statements
        size_t COLON_IDX = 0;
        {
          // check for colon ending the condition
          size_t idx_copy = words_idx;
          while (idx_copy < words.size()) {
            if (words[idx_copy].type == PUNCTUATION and
                words[idx_copy].p_type == COLON) {
              break;
            }
            ++idx_copy;
          }
          if (words[idx_copy].type != PUNCTUATION or
              words[idx_copy].p_type != COLON) {
            throw std::runtime_error("syntax error:\n" +
                                     words[words_idx].value);
          }
          COLON_IDX = idx_copy;
        }
        // we start parsing conditions here
        // if there are any PUNCTUATION symbols that need a pair, then we gotta
        // check for matching
        {
          std::stack<PUNCTUATION_TYPE> pair_check;
          size_t idx_copy = words_idx;
          while (idx_copy < COLON_IDX) {
            if (words[idx_copy].type == PUNCTUATION and
                (words[idx_copy].p_type == OPEN_SQR_BR or
                 words[idx_copy].p_type == OPEN_PARENTH)) {
              pair_check.push(words[idx_copy].p_type);
            } else if (pair_check.size() > 0 and
                       words[idx_copy].type == PUNCTUATION and
                       ((words[idx_copy].p_type == CLOSE_PARENTH and
                         pair_check.top() == OPEN_PARENTH) or
                        (words[idx_copy].p_type == CLOSING_SQR_BR and
                         pair_check.top() == OPEN_SQR_BR))) {
              pair_check.pop();
            }
          }
          assert(pair_check.size() == 0);
        }
        // we need a greedy matching solution so that it accurately matches what
        // expression is what type.
        // TODO
      } else if (w.value == "elif") {
      } else if (w.value == "else") {
      } else if (w.value == "def") {
      }
    }
  }
}

/* ------------------------------------------------------------ */

std::vector<data_node> parse(std::vector<word> words) {
  std::vector<data_node> nodes;
  for (size_t i = 0; i < words.size();) {
    word wrd = words[i];
    switch (wrd.type) {
    case IDENTIFIER: {

      data_node ident_node;
      ident_node.val.t = IDENTIFIER_NODE;
      ident_node.val.obj = new std::string;
      *(std::string *)ident_node.val.obj = wrd.value;
      nodes.push_back(ident_node);
      ++i;
    } break;
    case PUNCTUATION:
      switch (wrd.p_type) {
      case COLON:
        // COLON WILL EITHER BE A SLICE OR START OF NEW SCOPE OR TYPE ANNOTATION

        ++i;
        break;
      case OPEN_SQR_BR: {

        data_node temp_node;
        temp_node.val.t = WORD;
        temp_node.val.obj = new word;
        *(word *)temp_node.val.obj = wrd;
        nodes.push_back(temp_node);
        // [ needs to have an ending ]
        // indexing, dict key, ...
        size_t j = i + 1;
        size_t prev = i;
        while (j < words.size()) {
          switch (words[j].type) {
          case PUNCTUATION: {
            switch (words[j].p_type) {
            case COMMA: {
              if (j - prev <= 1) {
                throw std::runtime_error("syntax error1");
              }
              std::vector<data_node> temp_nodes = parse(std::vector<word>(
                  words.begin() + prev + 1, words.begin() + j));
              nodes.insert(nodes.end(), temp_nodes.begin(), temp_nodes.end());
              data_node comma_node;
              comma_node.val.t = WORD;
              comma_node.val.obj = new word;
              *(word *)comma_node.val.obj = words[j];
              nodes.push_back(comma_node);
              prev = j;
            } break;
            case CLOSING_SQR_BR: {
              goto exit_while;
            } break;
            }
          } break;
          }
          ++j;
        }
      exit_while : {}
        if (words[j].p_type == CLOSING_SQR_BR) {
          std::vector<data_node> temp_nodes = parse(
              std::vector<word>(words.begin() + prev + 1, words.begin() + j));
          nodes.insert(nodes.end(), temp_nodes.begin(), temp_nodes.end());
          data_node close_sqr_br;
          close_sqr_br.val.t = WORD;
          close_sqr_br.val.obj = new word;
          *(word *)close_sqr_br.val.obj = words[j];
          nodes.push_back(close_sqr_br);
        } else {
          std::cout << words[j].value << std::endl;
          throw std::runtime_error("syntax error2");
        }
        i = j + 1;
      } break;
      case CLOSING_SQR_BR:
        throw std::runtime_error("closing sqr br");
        break;
      case OPEN_PARENTH: {
        data_node open_parenth;
        open_parenth.val.t = WORD;
        open_parenth.val.obj = new word;
        *(word *)open_parenth.val.obj = wrd;
        nodes.push_back(open_parenth);
        size_t j = i + 1;
        while (j < words.size() and words[j].p_type != CLOSE_PARENTH) {
          ++j;
        }
        std::vector<data_node> in_between =
            parse(std::vector<word>(words.begin() + i + 1, words.begin() + j));
        for (data_node d : in_between) {
          nodes.push_back(d);
        }
        if (j < words.size()) {
          data_node close_parenth;
          close_parenth.val.t = WORD;
          close_parenth.val.obj = new word;
          *(word *)close_parenth.val.obj = words[j];
          nodes.push_back(close_parenth);
        } else {
          throw std::runtime_error("syntax error3");
        }
        i = j + 1;
      } break;
      case CLOSE_PARENTH:
        break;
      case QUOTE:
        break;
      case COMMA: {
        data_node comma_node;
        comma_node.val.t = WORD;
        comma_node.val.obj = new word;
        *(word *)comma_node.val.obj = wrd;
        nodes.push_back(comma_node);
        ++i;
      } break;
      case PERIOD:
        break;
      }
      break;
    case NUMBER_LITERAL: {
      data_node temp_node;
      temp_node.val.t = NUMBER;
      temp_node.val.obj = new NUM_TYPE;
      NUM_TYPE val_num = 0;
      *(NUM_TYPE *)temp_node.val.obj = parse_num_literal(wrd);
      nodes.push_back(temp_node);
      ++i;
    } break;
    case OPERATOR:
      switch (wrd.o_type) {
      case EQL_SIGN: {

        // ONLY CASE IS ASSIGNMENT
        data_node eql;
        eql.val.t = WORD;
        eql.val.obj = new word;
        *(word *)eql.val.obj = wrd;
        nodes.push_back(eql);
        ++i;
      } break;
      case PLUS_SIGN: {

        data_node plus;
        plus.val.t = WORD;
        plus.val.obj = new word;
        *(word *)plus.val.obj = wrd;
        nodes.push_back(plus);
        ++i;
      } break;
      default:
        throw std::runtime_error(std::to_string(wrd.o_type));
        break;
      }
      break;
    case STRING_LITERAL: {

      data_node temp_node;
      temp_node.val.t = STRING;
      temp_node.val.obj = new std::string(wrd.value);
      nodes.push_back(temp_node);
      ++i;
    } break;
    case KEYWORD:

      data_node keywrd;
      keywrd.val.t = KEYWORD_NODE;
      keywrd.val.obj = new std::string;
      *(std::string *)keywrd.val.obj = wrd.value;
      nodes.push_back(keywrd);
      ++i;
      break;
    case WHITESPACE:

      data_node whitespace_node;
      whitespace_node.val.t = WORD;
      whitespace_node.val.obj = new word;
      *(word *)whitespace_node.val.obj = wrd;
      nodes.push_back(whitespace_node);
      ++i;
      break;
    case COMMENT:

      ++i;
      break;
    default:
      throw std::runtime_error("default");
      break;
    }
  }
  return nodes;
}
