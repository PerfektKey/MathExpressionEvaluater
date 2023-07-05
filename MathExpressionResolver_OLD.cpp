#include <iostream>
#include <vector>
#include <string>
#include <inttypes.h>
#include <math.h>
#include "PJsonParser/Pparser.h"

enum class TokenTypes {
	OP_ADD = 0,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_POW,

	BIN_AND,
	BIN_OR,
	BIN_XOR,

	NUMBER,
	PARENT_CLOSING,
	PARENT_OPENING

};


struct token {

	TokenTypes type;
	int32_t value; // if type is number , the number is stored here
	uint8_t priority;
	token (TokenTypes type , uint8_t p , int32_t value=0) {
		this->type = type;
		this->priority = p;
		this->value = value;
	}
};
struct AST_Node;
struct AST_Node {
	int64_t value;
	TokenTypes type;

	AST_Node* left;
	AST_Node* right;

	AST_Node () {
		left = nullptr;
		right = nullptr;
	}
};

std::ostream& operator << ( std::ostream& , TokenTypes ); // prints a token example : {OP_ADD} -> add operator
std::string  TokenTypeToSrc (TokenTypes); // takes a token type and returns a string holfing its mathematcal representation examble : {OP_ADD} -> "+"
std::ostream& operator << ( std::ostream& , const token&  ); // prints a token in the format of tokenType : value
void printTabs (uint8_t num);
void printAST(AST_Node* , uint8_t=0);

int32_t getNumber (const std::string&, uint16_t*);
std::vector<token> lexer ( const std::string& ); // turns a mathematical expression into its tokens
int16_t getParentClosingIndex (std::vector<token>& , uint16_t);// takes the index of a parentheses start and gets its closing part

AST_Node* HandleParent(const std::vector<token>& , uint16_t);
AST_Node* TokensToAST (const std::vector<token>& , uint16_t=1);// takes the tokens and turns them into a abstact syntax tree
AST_Node* buildAST (const std::vector<token>&);

int64_t resolve ( AST_Node* ); // takes the tokens and calculatets its result

int64_t parseString(const std::string&); // does everything
void test();


int main () {
#if 1 
	test();
	return 0;
#endif
	std::string expr = " 5+6*(1+1)^2 ";
	expr = "5+2*6/3"; // something simpler
	expr = "2^8-1";
	std::cout << expr << "\n";
	std::vector<token> tokens = lexer (expr);
	for (const token& t : tokens)
		std::cout << t << "\n";
	std::cout << "\n---\n\n";

	AST_Node* root = buildAST(tokens);
	printAST (root);
	//std::cout << (*root) << "\n";

	std::cout << "result = " << resolve(root) << "\n";

	return 0;
}

void test () {
	using namespace PJsonParser;
	std::shared_ptr<JValue> V = parse ("Tests.json");
	JValue& value = *V;
	//std::cout << value.toString() << "\n";
	std::cout << value.getAsArray()->size() << "\n";
	for (int i = 0;i < value.getAsArray()->size();++i) {
		if (parseString(value[i]["expression"].toString() ) == value[i]["answer"].getAsLong()) {
			std::cout << "Test " << i << " passed\n" ; 
		}else {	
			std::cout << "Test " << i << " FAILED expression" << value[i]["expression"].toString() << " with expected result of: " << value[i]["answer"].toString() << "\n" ; 
		}
	}

}

int64_t parseString (const std::string& expr) {
	return resolve ( buildAST(lexer(expr)) );
}

// printing functions
std::ostream& operator << ( std::ostream& stream , TokenTypes type ) {
	
	switch ( type ) {
		case TokenTypes::OP_ADD:
			stream << "addition operator";
			break;
		case TokenTypes::OP_SUB:
			stream << "subtraction operator";
			break;
		case TokenTypes::OP_MUL:
			stream << "multiplication operator";
			break;
		case TokenTypes::OP_DIV:
			stream << "division operator";
			break;
		case TokenTypes::OP_POW:
			stream << "power operator";
			break;
		case TokenTypes::NUMBER:
			stream << "number type";
			break;
		case TokenTypes::PARENT_OPENING:
			stream << "opening parentheses";
			break;
		case TokenTypes::PARENT_CLOSING:
			stream << "closing parentheses";
			break;
	}

	return stream;
} 

std::string  TokenTypeToSrc (TokenTypes type) {
	std::string result;


	switch ( type ) {
		case TokenTypes::OP_ADD:
			result = "+";
			break;
		case TokenTypes::OP_SUB:
			result = "-";
			break;
		case TokenTypes::OP_MUL:
			result = "*";
			break;
		case TokenTypes::OP_DIV:
			result = "/";
			break;
		case TokenTypes::OP_POW:
			result = "^";
			break;
		case TokenTypes::NUMBER:
			result = " NUM ";
			break;
		case TokenTypes::PARENT_OPENING:
			result = "(";
			break;
		case TokenTypes::PARENT_CLOSING:
			result = ")";
			break;
	
	}
	return result;
}

std::ostream& operator  << ( std::ostream& stream , const token& token) {
	stream << token.type << " , v: " << token.value << " , p: " << (short)token.priority;
	return stream;
}

int16_t getParentClosingIndex (const std::vector<token>& expr , uint16_t startIndex) {
	uint8_t openings = 0;
	for (int i = startIndex;i < expr.size();++i) {
		if (expr.at(i).type == TokenTypes::PARENT_OPENING)
			++openings;
		if (expr.at(i).type == TokenTypes::PARENT_CLOSING)
			if (openings == 0) 
				return i;
			else
				--openings;
	}

	return -1;
}


void printTabs (uint8_t num) {
	for (int i = 0;i < num;i++)
		std::cout << "\t";
}
void printAST(AST_Node* node , uint8_t level) {
	if (node == nullptr){
		printTabs ( level );
		std::cout << "---<empty>---\n";
		return;
	}else {
		printTabs (level);
		std::cout << "type: " << node->type << "\n";
		printTabs (level);
		std::cout << "value: " << node->value << "\n";
	}

	if (node->type == TokenTypes::NUMBER)
		return;
	printTabs ( level );
	std::cout << "left\n";
	printAST ( node->left , level+1 );
	printTabs( level );
	std::cout << "right\n";
	printAST ( node->right , level+1 );

}

int32_t getNumber (const std::string& expr, uint16_t* index) {
	std::string num;// will hold the number

	while ( isdigit (expr.at(*index)) ) {
		num += expr.at(*index);
		++(*index);
		// bounds checking
		if ( (*index) >= expr.size() )
			break;
	} 
	//--(*index);
	return std::stoi (num);
}


std::vector<token> lexer ( const std::string& expr ) {
	std::vector <token> tokens;
	bool lastIsOperator = true;
	for ( uint16_t i = 0;i < expr.size();++i ) {
		lastIsOperator = true;
		if ( isdigit (expr.at(i))  ){
			lastIsOperator = false;
			tokens.push_back ( token (TokenTypes::NUMBER,0, getNumber(expr, &i) ) );
			//continue;
			if ( i >= expr.size() )
				break;
		} else if (lastIsOperator && i+1 < expr.size() && expr.at(i) == '-' && isdigit(expr.at(i+1)) ){
		
			lastIsOperator = false;
			++i;
			tokens.push_back ( token (TokenTypes::NUMBER,0, getNumber(expr, &i)*-1 ) );
			//continue;
			if ( i >= expr.size() )
				break;
		}	
		
		// the char is not a digit
		switch (expr.at(i) ) {
			case '+' :
				tokens.push_back ( token(TokenTypes::OP_ADD , 1) );
				break;
			case '-' :
				tokens.push_back ( token(TokenTypes::OP_SUB , 1) );
				break;
			case '*' :
				tokens.push_back ( token(TokenTypes::OP_MUL , 2) );
				break;
			case '/' :
				tokens.push_back ( token(TokenTypes::OP_DIV , 2) );
				break;
			case '^' :
				tokens.push_back ( token(TokenTypes::OP_POW , 3) );
				break;
			case '(' :
				tokens.push_back ( token(TokenTypes::PARENT_OPENING , 4) );
				break;
			case ')' :
				tokens.push_back ( token(TokenTypes::PARENT_CLOSING , 4) );
				break;
		} 
	}

	return tokens;
}

AST_Node* TokensToAST ( const std::vector<token>& expr , uint16_t index ) {
	
	// bounds checking
	if ( index >= expr.size() ){
		// \033[31m sets the color to red and \033[0m resets the color
		std::cout << "\033[31m[ERROR]  index " << index << " out of bounds of expression vector with size " << expr.size() << "\n\033[0m";
		exit (1);
	}
	
	AST_Node* root = new AST_Node ();
	
	// set the basic type of the node
	root->type  = expr.at(index).type;
	root->value = expr.at(index).value;

	// a NUMBER node doesnt have to do anything more so just return
	if (root->type == TokenTypes::NUMBER)
		return root;
	
	// now the node can only be a operator like +,-,*,/ or something like ()
	// before and after any operator a number must be otherwise its an error , for that
	// the index has to have 1 index below and up
	if ( index == 0 && index != expr.size() ) {
		std::cout << "\033[31m[ERROR]  index " << index << " cant be zero or the last index  when parsing operators\033[0m\n";
		exit (1);
	}
	if ( expr.at(index-1).type != TokenTypes::PARENT_CLOSING || expr.at(index+1).type != TokenTypes::PARENT_OPENING )
	if ((expr.at(index-1).type != TokenTypes::NUMBER || expr.at(index+1).type != TokenTypes::NUMBER) ) {
	 	std::cout << "\033[31m [ERROR] before and after an operator only numbers are allowed, expression:\n\033[31m";
		// print the whole expression with one before and after the operator in red
		for (int ei = 0;ei < expr.size();++ei)
			std::cout << ((ei >= index-1 && ei <= index+1) ? "\033[31m" : "\033[0m"  ) << TokenTypeToSrc ( expr.at(ei).type );
		std::cout << "\n";
		exit (1);
	}
	//
	// 5 + 6 * 3
	//  6 * 3
	// 

	root->left  = TokensToAST (expr , index-1);
	root->right = TokensToAST (expr , index+1);


	return root;
}

AST_Node* buildAST (const std::vector<token>& expr) {
	uint8_t startingIndex = 3; // the starting index for the for loop
	AST_Node* LeftParentRoot = nullptr;
	AST_Node* root = TokensToAST (expr, 1 );
	/*if ( expr.at(0).type == TokenTypes::PARENT_OPENING ) {
		int16_t closing = getParentClosingIndex(expr, 1);
		std::cout << closing << "::\n";
		if (closing == -1) {
			std::cerr <<"\033[31mOn Index 0 a Parentheses was opened but it never got closed\033[31m\n";
			exit(1);
		}
		std::vector<token> subBuild = { expr.begin()+1 , expr.begin()+closing };
		LeftParentRoot = buildAST (subBuild);
		///root = LeftParentRoot;
		//LeftParentRoot = nullptr;
		// (1+2)*(3+4)
		startingIndex = closing + 1;
		return LeftParentRoot;
	}*/
	for (int i = startingIndex;i < expr.size();i+=2) {
		AST_Node* ret = nullptr;
		if (LeftParentRoot != nullptr) {
			ret = LeftParentRoot;
			LeftParentRoot = nullptr;
		}else
			ret = TokensToAST (expr , i);
		//return root;
		if ( expr.at(i-2).priority == expr.at(i).priority ) {
			//std::cout << "equal\n";
			ret->left = root;
			root = ret;
		}else if (expr.at(i-2).priority < expr.at(i).priority ) {
			//std::cout << "bigger next\n";
			root->right = ret;
		}else {
			//std::cout << "bigger prev\n";
			ret->left = root;
			root = ret;
		}
	}
	return root;
}


int64_t resolve (AST_Node* node) {
	if (node == nullptr) {
		std::cout << "nullptr node not accepted";
		exit(1);
	}

	if ( node->type == TokenTypes::NUMBER ) {
		return node->value;
	}
	
	int64_t num1 = resolve(node->left);
	int64_t num2 = resolve(node->right);
	switch ( node->type ) {
		case TokenTypes::OP_ADD :
			return num1 + num2;
		case TokenTypes::OP_SUB :
			return num1 - num2;	
		case TokenTypes::OP_MUL :
			return num1 * num2;	
		case TokenTypes::OP_DIV :
			return num1 / num2;
		case TokenTypes::OP_POW:
			return pow (num1 , num2);	
	}
	return 0;
}
