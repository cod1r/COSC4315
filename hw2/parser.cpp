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
        data_node colon_node;
        colon_node.val.t = WORD;
        colon_node.val.obj = new word;
        *(word *)colon_node.val.obj = wrd;
        nodes.push_back(colon_node);
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
      case LESS_THAN:
      case LESS_THAN_EQL_TO:
      case EQL_SIGN:
      case PLUS_SIGN:
      case EQUAL_BOOL: {
        data_node eql_bool;
        eql_bool.val.t = WORD;
        eql_bool.val.obj = new word;
        *(word *)eql_bool.val.obj = wrd;
        nodes.push_back(eql_bool);
        ++i;
      } break;
      default:
        throw std::runtime_error("unknown operator: " + wrd.value);
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
    case NEWLINE:

      data_node newline_node;
      newline_node.val.t = WORD;
      newline_node.val.obj = new word;
      *(word *)newline_node.val.obj = wrd;
      nodes.push_back(newline_node);
      ++i;
      break;
    case INDENT:

      data_node indent_node;
      indent_node.val.t = WORD;
      indent_node.val.obj = new word;
      *(word *)indent_node.val.obj = wrd;
      nodes.push_back(indent_node);
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
