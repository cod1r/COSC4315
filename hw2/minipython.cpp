#include "lexer.h"
#include "parser.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
int main(int argc, char *argv[]) {
  if (argc < 2) {
    exit(EXIT_FAILURE);
  }
  std::vector<word> words = lexer(argv[1]);
  std::vector<data_node> nodes = parse(words);
  for (data_node dn : nodes) {
    switch (dn.val.t) {
		case UNKNOWN:
			std::cout << *(std::string*)dn.val.obj << std::endl;
			break;
    case NUMBER:
      std::cout << *(long long *)dn.val.obj << std::endl;
      break;
    case STRING:
      std::cout << *(std::string *)dn.val.obj << std::endl;
      break;
		case WORD:
			std::cout << (*(word*)dn.val.obj).value << std::endl;
			break;
    }
  }
}
