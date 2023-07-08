#include "src/MathExpressionSolver.h"
#include <thread>
#include <unistd.h>

void engine ();

int main () {
  PMES::Solver solver;
  std::cout << "error code for 5+5:   " << solver.isValid("5+5"  ) << "\n";
  std::cout << "error code for 5+5+:  " << solver.isValid("5+5+" ) << "\n";
  std::cout << "error code for 5+5++: " << solver.isValid("5+5++") << "\n";
  std::cout << "error code for 5+55:  " << solver.isValid("5+55" ) << "\n";

  std::thread ENGINE (engine);

	ENGINE.join();

	return 0;
}


void engine () {
	PMES::Solver solver;
	std::string expr;
	while ( 1 ) {
	
		std::cout << ">>> ";
		std::getline (std::cin , expr);
		if (expr == "exit")
			break;
    		std::cout << "input: " << expr << "\n";
		uint32_t err = solver.isValid(expr); 
    		if (err > 1) {
			std::cout << "\033[31minvalid expression\n";
			std::cout << "error code: " << err << "\n\033[0m";
			expr.clear();
			continue;
		}
		
			
		std::cout << solver.parse(expr) << "\n";
		expr.clear();

	}
}
