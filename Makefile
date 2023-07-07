CC=clang++
FLAGS=-std=c++20 -Wall -g

all:

Test:
	$(CC) $(FLAGS) -o test tester.cpp src/MathExpressionSolver.cpp src/PJsonParser/parser.cpp

