minipython: minipython.o lexer.o parser.o interpreter.o
	clang++ minipython.o lexer.o parser.o interpreter.o -o minipython -g
minipython.o: minipython.cpp
	clang++ -c minipython.cpp -Wall -o minipython.o -g
lexer.o: lexer.cpp
	clang++ -c lexer.cpp -Wall -o lexer.o -g
parser.o: parser.cpp
	clang++ -c parser.cpp -Wall -o parser.o -g
interpreter.o: interpreter.cpp
	clang++ -c interpreter.cpp -Wall -o interpreter.o -g
format:
	clang-format -i *.cpp *.h
