#include "src/MathExpressionSolver.h"
#include "src/PJsonParser/Pparser.h"
#include <algorithm>

    
int main () {
	namespace PJP = PJsonParser;

	std::shared_ptr<PJP::JValue> V = PJP::parse ("Tests.json");
	PJP::JValue& val = *V;

	PMES::Solver solver("");
	for (int i = 0;i < val.getAsArray()->size();++i) {
		std::string str = val[i]["expression"].toString();
		str.erase ( std::remove(str.begin() , str.end() , '\"') , str.end() );
		PMES::VALUE value =  solver.parse (str);
	
		// fabs = float absolute value
		// doing "fabs(float - float) < EPSILON" is a better way of ensurung equality for decimals	
		if ( fabs(value - (PMES::VALUE)(val[i]["answer"].getAsDouble())) < 1e-5 )
			std::cout << "\033[32mPASSED TEST " << i << "\n\033[0m";
		else
			std::cout << "\033[31mFAILED TEST " << i << " expression: " << str << " expected " << val[i]["answer"].getAsDouble() << " but got " << value  << "\n\033[0m";	
	}

	return 0;
}

