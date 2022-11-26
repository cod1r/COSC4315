#include "interpreter.h"
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
  // for (word w : words) {
  //	std::cout << w.value << std::endl;
  // }
  std::vector<data_node> nodes = parse(words);
  run(nodes);
}
