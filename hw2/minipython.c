#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "words.h"
#include "lexer.h"
int main(int argc, char *argv[])
{
	if (argc < 2) {
		exit(EXIT_FAILURE);
	}
	lexer(argv[1]);
}
