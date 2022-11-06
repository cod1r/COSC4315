#include "parser.h"
#include "lexer.h"
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>

Expression parse2(std::vector<word> words) { Expression program; }

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
        while (j < words.size() and words[j].p_type != CLOSING_SQR_BR) {
          switch (words[j].type) {
          case PUNCTUATION: {
            switch (words[j].p_type) {
            case COMMA: {
              if (j - prev <= 1) {
                throw std::runtime_error("syntax error");
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
            }
          } break;
          }
          ++j;
        }
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
          throw std::runtime_error("syntax error");
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
        int j = i + 1;
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
          throw std::runtime_error("syntax error");
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
      temp_node.val.obj = new long long;
      long long val_num = 0;
      for (size_t num_idx = 0; num_idx < wrd.value.length(); ++num_idx) {
        val_num += (wrd.value[num_idx] - '0') *
                   std::pow(10, wrd.value.length() - 1 - num_idx);
      }
      *(long long *)temp_node.val.obj = val_num;
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
