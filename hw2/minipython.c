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
	words wds = lexer(argv[1]);
	for (int i = 0; i < wds.length; ++i) {
		switch (wds.wds[i]->type) {
IDENTIFIER: {
							printf("IDENTIFIER\n");
						}
			default: break;
		}
	}
}
