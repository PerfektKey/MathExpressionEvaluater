CC=clang++
FLAGS=-std=c++20 -Wall -g

all:

engine:
	$(CC) $(FLAGS) -pthread -o Engine Engine.cpp src/MathExpressionSolver.cpp src/PJsonParser/parser.cpp
test:
	$(CC) $(FLAGS) -o test tester.cpp src/MathExpressionSolver.cpp src/PJsonParser/parser.cpp

