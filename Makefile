CC=clang++


all:
	$(CC) -std=c++20 -Wall -o MER MathExpressionSolver.cpp PJsonParser/parser.cpp
	./MER
