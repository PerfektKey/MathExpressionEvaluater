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
	OPEN_PARENT,
	CLOSE_PARENT
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
		case TOKENS::POW:
			stream << "power symbol";
			return stream;
		case TOKENS::OPEN_PARENT:
			stream << "left parentheses";
			return stream;
		case TOKENS::CLOSE_PARENT:
			stream << "right parentheses";
			return stream;
	}
	return stream;
}

// the struct for holding the token information
struct tokenHolder {
public:
	std::string src;
	TOKENS token;
	bool isOperator;
	int32_t num;		// will only be used if token == TOKENS::NUMBER
	tokenHolder (TOKENS t, bool isOperator=true, uint32_t n=0 ){
		token = t;
		num = n;
		this->isOperator = isOperator;
	}
};

int getParentClosingIndex (const std::vector<tokenHolder>& ,uint8_t startingIndex );
int getNumber(const std::string&, uint16_t&);			// gets a number
std::vector<tokenHolder> tokenizer (const std::string&);	// takes a string and returns a array of token holders
int64_t resolve ( std::vector<tokenHolder>& , uint8_t=0 );			// resolves a array of token holders to a number

int main (int argc, char** argv) {
	std::string expr = "5+6*(2+1)";
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

int getParentClosingIndex (const std::vector<tokenHolder>& tokens , uint8_t startingIndex) {
	
	int OpenParentsFound = 0;
	for (int i = startingIndex;i < tokens.size();++i) {
		if (tokens.at(i).token == TOKENS::OPEN_PARENT)
			++OpenParentsFound;
		else if ( tokens.at(i).token == TOKENS::CLOSE_PARENT ) {
			if (OpenParentsFound > 0)
				--OpenParentsFound;
			else{
				return i;
			}
		
		}
	}
	return -1;
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

		tokenHolder token(TOKENS::ADD);
		token.src = expr.at(location);
		if (expr.at(location) >= '0' && expr.at(location) <= '9' ){	// if the character is a digit get that digit
			token.num = getNumber(expr, location);
			token.src = std::to_string( token.num );
			token.token = TOKENS::NUMBER;
			token.isOperator = false;
			tokens.push_back( token );
			LastIsOperator = false;				// this token is not a operator
			token.src = std::to_string ( token.num );
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
				token.token = TOKENS::ADD;
				tokens.push_back( token );
				break;
			case '-' :
				token.token = TOKENS::SUB;
				tokens.push_back( token );
				break;
			case '*' :
				token.token = TOKENS::MUL;
				tokens.push_back( token );
				break;
			case '/' :
				token.token = TOKENS::DIV;
				tokens.push_back( token );
				break;
			case '^' :
				token.token = TOKENS::POW;
				tokens.push_back( token );
				break;
			case '(':
				token.token = TOKENS::OPEN_PARENT;
				tokens.push_back( token );
				break;
			case ')':
				token.token = TOKENS::CLOSE_PARENT;
				tokens.push_back( token );
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
	/*if ( tokens.at(tokens.size()-1).isOperator ){
		std::cout << "last operator does not have a opearant";
		exit(1);
	}*/
	return tokens;
}




int64_t resolve(std::vector<tokenHolder>& tokens , uint8_t StartingIndex){
	
	for (int i = 0;i < tokens.size();++i) {
		// if the token is not a operator or parentheses continue;
		if ( !tokens.at(i).isOperator && tokens.at(i).token != TOKENS::OPEN_PARENT )
			continue;
		// error checking
		if (i > 0 && (tokens.at(i-1).token != TOKENS::NUMBER || tokens.at(i+1).token != TOKENS::NUMBER) ){
			std::cout << i << " - " << tokens.at(i-1).token << " & " << tokens.at(i+1).token << " =:Two operator after another is not allowed!\n";
			// print the expression with the errors in red
			for (int spi = 0; spi < tokens.size();++spi)
					std::cout << ( (spi >= i-1 && spi <= i+1)? "\033[1;31m" : "\033[0m" ) << tokens.at(spi).src << " ";
			std::cout << "\n";
			exit(1);
		}
		
		if ( tokens.at(i).token == TOKENS::OPEN_PARENT ){
			//tokens.erase ( tokens.begin()+i, tokens.begin()+getParentClosingIndex(tokens, i) );
			tokens.erase( tokens.begin()+i+1 , tokens.begin()+getParentClosingIndex(tokens, i+1)+1 );
			continue;
		}
		// check if the previous and next token are numbers: if not throw error
		std::cout << tokens.at(i).token << "\n";
	}
	
	
	return 0;
}
