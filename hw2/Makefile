minipython: minipython.o lexer.o parser.o interpreter.o
	g++ minipython.o lexer.o parser.o interpreter.o -o minipython
minipython.o: minipython.cpp
	g++ -c minipython.cpp -Wall -o minipython.o
lexer.o: lexer.cpp
	g++ -c lexer.cpp -Wall -o lexer.o
parser.o: parser.cpp
	g++ -c parser.cpp -Wall -o parser.o
interpreter.o: interpreter.cpp
	g++ -c interpreter.cpp -Wall -o interpreter.o
format:
	clang-format -i *.cpp *.h
