#include <iostream>
#include <vector>
#include <string>

enum class TOKENS {
	NUMBER = 0,
	ADD,
	SUB,
	MUL,
	DIV
};

std::ostream& operator<< (std::ostream& stream, TOKENS t) {
	switch (t) {
		case TOKENS::NUMBER:
			stream << "number" ;
			return stream;
		case TOKENS::ADD:
			stream << "addition symbol";
			return stream;
		case TOKENS::SUB:
			stream << "subtraction symbol";
			return stream;
		case TOKENS::MUL:
			stream << "multiplication symbol";
			return stream;
		case TOKENS::DIV:
			stream << "divide symbol";
			return stream;
	}
	return stream;
}


struct tokenHolder {
public:
	TOKENS token;
	bool isOperator;
	uint32_t num;
	tokenHolder (TOKENS t, bool isOperator=true, uint32_t n=0 ){
		token = t;
		num = n;
		this->isOperator = isOperator;
	}
};


int getNumber(const std::string&, uint16_t&);
std::vector<tokenHolder> tokenizer (const std::string&);
uint64_t resolve (std::vector<tokenHolder>&);

int main (int argc, char** argv) {
	std::string expr = "5+6*3";
	if (argc > 1)
		expr = argv[1];
	std::vector<tokenHolder> temp = tokenizer (expr);
	std::cout << "expression is " << expr << "\n";
	for (tokenHolder& th : temp) {
		std::cout << th.token << " : " << th.num << "\n";
	}
	std::cout << " = " << (long long)resolve (temp) << "\n";
	return 0;
}


int getNumber (const std::string& expr, uint16_t& location) {
	std::string num;
	while ( isdigit(expr.at(location)) ) {
		num += expr.at(location);
		location++;
		if (location >= expr.size())
			break;
	}
	return std::atoi(num.c_str());
}

std::vector<tokenHolder> tokenizer (const std::string& expr) { 
	uint16_t location = 0;// the char location in the string
	std::vector<tokenHolder> tokens;
	bool LastIsOperator = false;
	while (location < expr.size()) {
		if (expr.at(location) >= '0' && expr.at(location) <= '9' ){
			tokens.push_back(
				tokenHolder(
					TOKENS::NUMBER,
					false,
					getNumber (expr, location)
					    )
					);
			LastIsOperator = false;
			continue;
		}

		// get negative number

		switch (expr.at(location)) {
			case '+' :
				tokens.push_back( tokenHolder(TOKENS::ADD) );
				break;
			case '-' :
				tokens.push_back( tokenHolder(TOKENS::SUB) );
				break;
			case '*' :
				tokens.push_back( tokenHolder(TOKENS::MUL) );
				break;
			case '/' :
				tokens.push_back( tokenHolder(TOKENS::DIV) );
				break;
		}
		LastIsOperator = true;
		location++;
	}
	return tokens;
}


uint64_t resolve (std::vector<tokenHolder>& tokens){
	if (tokens.at(0).token != TOKENS::NUMBER) {
		std::cout << "first element is not a number";
		exit(1);
	}

	// look for multiplication and divisions , do them and replace their tokens with new number tokens
	for (size_t loc = 1;loc < tokens.size();loc+=2) {
		if (tokens.at(loc).token == TOKENS::MUL || tokens.at(loc).token == TOKENS::DIV ){
			// mult it
			uint32_t num = tokens.at(loc-1).num;
			switch (tokens.at(loc).token) {
				case TOKENS::MUL:
					num *= tokens.at(loc+1).num;
					break;
				case TOKENS::DIV:
					num /= tokens.at(loc+1).num;
					break;
			}

			// if the expr was 3 * 6 , replace the tokens for 3 * 6 with 18
			// the 3 is loc-1 , the * is loc , the 6 is loc + 1
			/*std::cout << "after: " << tokens.at(loc+2).token << " : " << tokens.at(loc+2).num << "\n";
			std::cout << "changing: " << tokens.at(loc-1).token << " : " << tokens.at(loc-1).num << "\n";
			std::cout << "deleting: " << tokens.at(loc-0).token << " : " << tokens.at(loc-0).num << "\n";
			std::cout << "deleting: " << tokens.at(loc+1).token << " : " << tokens.at(loc+1).num << "\n";*/
			// replace the number with the result and removing the operants
			tokens.at(loc-1).num = num;
			tokens.erase(tokens.begin()+loc);
			tokens.erase(tokens.begin()+loc);

			/*std::cout << "changed expression: \n";
			for (tokenHolder& th : tokens)
				std::cout << "\t"  << th.token << " : " << th.num << "\n";*/
			loc = 1;
		}
	}

	std::cout << "\n-\n";
	for (const tokenHolder& th : tokens) 
		std::cout << th.token << " : " << th.num << "\n";
	std::cout << "\n-\n";

	uint64_t res = tokens.at(0).num;
	for (size_t loc = 1;loc < tokens.size();loc+=2) {
		if ( !(tokens.at(loc).isOperator && tokens.at(loc+1).token == TOKENS::NUMBER) ) {
			std::cout << "token not right: " << tokens.at(loc).token << " , " << tokens.at(loc+1).token << "\n";
			exit(1);
		}
		switch (tokens.at(loc).token) {
			case TOKENS::ADD:
				res += (tokens.at(loc+1).num);
				break;
			case TOKENS::SUB:
				res -= (tokens.at(loc+1).num);
				break;
			case TOKENS::MUL:
				res *= (tokens.at(loc+1).num);
				break;
			case TOKENS::DIV:
				res /= (tokens.at(loc+1).num);
				break;
		}
	}
	return res;

}
