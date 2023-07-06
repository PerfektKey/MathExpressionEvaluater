CC=clang++


all:
	$(CC) -std=c++20 -Wall -o MER MathExpressionSolver.cpp PJsonParser/parser.cpp
	./MER
old:
	$(CC) -std=c++20 -Wall -o OLD_MER MathExpressionResolver_OLD.cpp PJsonParser/parser.cpp
