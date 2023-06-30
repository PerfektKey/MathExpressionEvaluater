#include <iostream>
#include <vector>
#include <string>
#include <cstdint> // for things like uint32_t

enum class TOKENS {
	NUMBER = 0,
	ADD,
	SUB,
	MUL,
	DIV,
	POW,
};

// printing a token
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

// the struct for holding the token information
struct tokenHolder {
public:
	TOKENS token;
	bool isOperator;
	int32_t num;		// will only be used if token == TOKENS::NUMBER
	tokenHolder (TOKENS t, bool isOperator=true, uint32_t n=0 ){
		token = t;
		num = n;
		this->isOperator = isOperator;
	}
};

int getParenthesesClosingPosition ();
int getNumber(const std::string&, uint16_t&);			// gets a number
std::vector<tokenHolder> tokenizer (const std::string&);	// takes a string and returns a array of token holders
uint64_t resolve (std::vector<tokenHolder>&);			// resolves a array of token holders to a number

int main (int argc, char** argv) {
	std::string expr = "5+6*3";
	if (argc > 1)		// if we have more than 1 argument given: resolve it
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
		num += expr.at(location);	// add the digit to the digit string
		location++;			
		if (location >= expr.size()) 	// if location is at the end of the string break from the loop
			break;
	}
	return std::atoi(num.c_str());
}

std::vector<tokenHolder> tokenizer (const std::string& expr) { 
	uint16_t location = 0;// the char location in the string
	std::vector<tokenHolder> tokens;
	bool LastIsOperator = false;
	while (location < expr.size()) {
		if (expr.at(location) >= '0' && expr.at(location) <= '9' ){	// if the character is a digit get that digit
			tokens.push_back(
				tokenHolder(
					TOKENS::NUMBER,			// set the token
					false,				// it is not a operator
					getNumber (expr, location)	// get the number , getNumber modifies the location so no need to do that here
					    )
					);
			LastIsOperator = false;				// this token is not a operator
			continue;
		}

		// get negative number
		// if the last token is a operator and this one is a minus do this and the next char is a digit; 
		// if yes get that number and multiplie it by -1
		if (LastIsOperator && expr.at(location) == '-' && isdigit(expr.at(location+1))  ) {
			location++; 				// cant do getNumber(expr,location+1) because that would be a ivalue referenz
			int num = getNumber (expr, location);
			num *= -1;
			tokens.push_back( tokenHolder(TOKENS::NUMBER,false, num ) );
			continue;
		}

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
			case '^' :
				tokens.push_back ( tokenHolder (TOKENS::POW) );
				break;
			default:
				std::cout << "[WARNING] illegal character detected: " << expr.at(location) << " at index " << location << "\n";
				break;
		}
		LastIsOperator = true;
		location++;
	}
	// if the last token is a operator a error is there
	// before that check if tokens is bigger than 0
	if (tokens.size() == 0){
		std::cout << "Invalid expression \" " << expr << " \" no tokene detected\n";
		exit(1);
	}
	if ( tokens.at(tokens.size()-1).isOperator ){
		std::cout << "last operator does not have a opearant";
		exit(1);
	}
	return tokens;
}


uint64_t resolve (std::vector<tokenHolder>& tokens){
	std::cout << "\tresolve is not finished \n";
	if (tokens.at(0).token != TOKENS::NUMBER) {
		std::cout << "first element is not a number";
		exit(1);
	}
	int64_t result = tokens.at(0).num;

	for (int loc = 1;loc < tokens.size();loc+=2) {

		// check if loc+1 exists
		if (loc+1 >= tokens.size())
			break;
		//check if a next operation is availibale
		// 5 + 6 * 3
		// 5 + 6 has a next operation : 6 * 3
		// 6 * 3 has no next operation
		if ( !(loc+2 < tokens.size() && tokens.at(loc+2).isOperator) ){
		
		}

	}

	return result;

	// old way
	// look for multiplication and divisions , do them and replace their tokens with new number tokens
	for (size_t loc = 1;loc < tokens.size();loc+=2) {
		//check if loc+1 exists
		if (loc+1 >= tokens.size())
			break;
			
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
			/*
			std::cout << "after: " << tokens.at(loc+2).token << " : " << tokens.at(loc+2).num << "\n";
			std::cout << "changing: " << tokens.at(loc-1).token << " : " << tokens.at(loc-1).num << "\n";
			std::cout << "deleting: " << tokens.at(loc-0).token << " : " << tokens.at(loc-0).num << "\n";
			std::cout << "deleting: " << tokens.at(loc+1).token << " : " << tokens.at(loc+1).num << "\n";
			*/
			// replace the number with the result and removing the operants
			tokens.at(loc-1).num = num;
			tokens.erase(tokens.begin()+loc);
			tokens.erase(tokens.begin()+loc);

			/*
			std::cout << "changed expression: \n";
			for (tokenHolder& th : tokens)
				std::cout << "\t"  << th.token << " : " << th.num << "\n";
			*/
			loc = 1;
		}
	}
	
	// 5 + 6 * 3

	std::cout << "\n-\n";
	for (const tokenHolder& th : tokens) 
		std::cout << th.token << " : " << th.num << "\n";
	std::cout << "\n-\n";

	uint64_t res = tokens.at(0).num;
	for (size_t loc = 1;loc < tokens.size();loc+=2) {
		// check if loc+1 is a valid index
		if (loc+1 >= tokens.size())
			break;
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
