minipython: minipython.o lexer.o parser.o interpreter.o
	clang++ minipython.o lexer.o parser.o interpreter.o -o minipython
minipython.o: minipython.cpp
	clang++ -c minipython.cpp -Wall -o minipython.o
lexer.o: lexer.cpp
	clang++ -c lexer.cpp -Wall -o lexer.o
parser.o: parser.cpp
	clang++ -c parser.cpp -Wall -o parser.o
interpreter.o: interpreter.cpp
	clang++ -c interpreter.cpp -Wall -o interpreter.o
format:
	clang-format -i *.cpp *.h
