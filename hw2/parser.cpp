#include "parser.h"
#include "lexer.h"
#include <cmath>
#include <iostream>
#include <map>
#include <string>
#include <vector>

std::vector<data_node> parse(std::vector<word> words) {
  std::vector<data_node> nodes;
  for (size_t i = 0; i < words.size();) {
    word wrd = words[i];
    switch (wrd.type) {
    case IDENTIFIER: {
      std::cout << "I: " << wrd.value << "\n";
      data_node ident_node;
      ident_node.val.t = UNKNOWN;
			ident_node.val.obj = new std::string;
			*(std::string*)ident_node.val.obj = wrd.value;
      nodes.push_back(ident_node);
      ++i;
    } break;
    case PUNCTUATION:
      switch (wrd.p_type) {
      case COLON:
        // COLON WILL EITHER BE A SLICE OR START OF NEW SCOPE OR TYPE ANNOTATION
        std::cout << ":\n";
        ++i;
        break;
      case OPEN_SQR_BR: {
        std::cout << "[\n";
				data_node temp_node;
				temp_node.val.t = WORD;
				temp_node.val.obj = new word;
				*(word*)temp_node.val.obj = wrd;
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
							*(word*)comma_node.val.obj = words[j];
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
					*(word*)close_sqr_br.val.obj = words[j];
					nodes.push_back(close_sqr_br);
        }
        i = j + 1;
      } break;
      case CLOSING_SQR_BR:
        throw std::runtime_error("");
        break;
      case OPEN_PARENTH: {
        std::cout << "(\n";
        int j = i + 1;
        while (j < words.size() and words[j].p_type != CLOSE_PARENTH) {
          ++j;
        }
        std::vector<data_node> in_between =
            parse(std::vector<word>(words.begin() + i + 1, words.begin() + j));
        for (data_node d : in_between) {
          nodes.push_back(d);
        }
        i = j + 1;
      } break;
      case CLOSE_PARENTH:
        break;
      case QUOTE:
        break;
      case COMMA:
				throw std::runtime_error("FOUND");
        break;
      case PERIOD:
        break;
      }
      break;
    case NUMBER_LITERAL: {
      std::cout << "N\n";
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
        std::cout << "=\n";
        // ONLY CASE IS ASSIGNMENT
				data_node eql;
				eql.val.t = WORD;
				eql.val.obj = new word;
				*(word*)eql.val.obj = wrd;
        ++i;
      } break;
      case PLUS_SIGN: {
        std::cout << "+\n";
				data_node plus;
				plus.val.t = WORD;
				plus.val.obj = new word;
				*(word*)plus.val.obj = wrd;
        ++i;
      } break;
      default:
        throw std::runtime_error(std::to_string(wrd.o_type));
        break;
      }
      break;
    case STRING_LITERAL: {
      std::cout << "STR LIT: " << wrd.value << '\n';
      data_node temp_node;
      temp_node.val.t = STRING;
      temp_node.val.obj = new std::string(wrd.value);
      nodes.push_back(temp_node);
      ++i;
    } break;
    case KEYWORD:
      std::cout << "K: " << wrd.value << '\n';
      ++i;
      break;
    case WHITESPACE:
      std::cout << "W\n";
      ++i;
      break;
    case COMMENT:
      std::cout << wrd.value << '\n';
      ++i;
      break;
    default:
      throw std::runtime_error("default");
      break;
    }
  }
  return nodes;
}
