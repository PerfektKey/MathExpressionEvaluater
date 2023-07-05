#include "MathExpressionSolver.h"


namespace PMES  {

	
	std::ostream& operator << (std::ostream& stream , TokenType tok) {
		switch (tok) {
			case TokenType::NUMBER:
				stream << "Number type";
				break;
			case TokenType::OP_ADD:
				stream << "Addition operation";
				break;
			case TokenType::OP_SUB:
				stream << "Subtraction operation";
				break;
			case TokenType::OP_MUL:
				stream << "Multiplication operation";
				break;
			case TokenType::OP_DIV:
				stream << "division operation";
				break;
			
		
		}
		return stream;
	}

	std::ostream& operator << (std::ostream& stream , const token& tok) {
		stream << "Token:\n";
		stream << "\ttype: "     << tok.type    << "\n";
		stream << "\tvalue: "    << tok.value    << "\n";
		stream << "\tpriority: " << (unsigned int)tok.priority << "\n";
		return stream;
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

		if (node->type == TokenType::NUMBER)
			return;
		printTabs ( level );
		std::cout << "left\n";
		printAST ( node->left , level+1 );
		printTabs( level );
		std::cout << "right\n";
		printAST ( node->right , level+1 );

	}
	

	Solver::Solver (std::string src) {
		this->srcString = src;
		this->root = new AST_Node();
	}
	Solver::~Solver () {
		delete root;
	}

	int64_t Solver::getNumber (const std::string& src , uint32_t* ix) {
		std::string temp;

		uint32_t& index = (*ix);

		while ( index < src.size() && isdigit(src.at(index)) ) {
			temp += src.at(index);
			++(index);
		}
		//--(index);
		return stoi (temp);
	
	}

	
	void Solver::lex () {
		this->lex (srcString);
	}
	void Solver::lex (const std::string& src) {
		uint32_t index = 0;
		bool lastIsNumber = false;
		while ( index < src.size() ) {
			
			if ( isdigit( src.at(index) ) ){
				lastIsNumber = true;
				int64_t num = getNumber (src , &index);
				tokens.push_back (token(TokenType::NUMBER , 0 , num));
				continue;
			}
			if (index+1 < src.size() && src.at(index) == '-' && isdigit( src.at(index+1) ) ){
				lastIsNumber = true;
				++index;
				int64_t num = getNumber (src , &index);
				tokens.push_back (token(TokenType::NUMBER , 0 , num*-1));
				continue;
			}

			switch ( src.at(index) ) {
				case '+':
					tokens.push_back (token(TokenType::OP_ADD, 1));
					break;
				case '-':
					tokens.push_back (token(TokenType::OP_SUB, 1));
					break;
				case '*':
					tokens.push_back (token(TokenType::OP_MUL, 1));
					break;
				case '/':
					tokens.push_back (token(TokenType::OP_DIV, 1));
					break;
				default:
					std::cout << "illegal character in Solver lex " << src.at(index) << "\n";
					break;
			}


			++index;
		}
	}

	void Solver::printTokens () {
		for (const token& tok : tokens)
			std::cout << tok << "\n";	
	}

	token Solver::eat (TokenType type) {
		token tok = tokens.at(0);
		tokens.erase(tokens.begin());
		if (type != TokenType::NUL) {
			if (tok.type != type) {
				std::cerr << "\033[31m poped token does not have requested type in Solver::eat()\033[31m\n";
				exit (1);
			} else
				return tok;
		}
		return tok;
	}
	bool Solver::EOE () {
		return tokens.size() == 0;
	}

	AST_Node* Solver::getOPNum(const token& tok) {
		if (tok.type == TokenType::NUMBER) {
			// when the token is just a number; get the values and continue
			return new AST_Node (tok.type , tok.value);
		} else if (tok.type == TokenType::PARENT_OPENING) {
			// when the token is a parentheses build the expression
			return  build (true);
		}
		return nullptr;
	}

	
	void Solver::build () {
		root = this->build ( false );
	}
	AST_Node* Solver::build ( bool openParent ) {
		/*  REFACTOR code: 
		 *  	left is only useful in the first iteration so do refactor it so that it doesnt need it in the while loop
		 */

		AST_Node* root = nullptr;
		AST_Node* left = nullptr;
		int16_t previous_operator_priority = -1; // has the last operators priority , -1 stands for the first iteration

		// if the left node is empty configure it
		if (left == nullptr) {
		       	token tok = eat();
			left = getOPNum(tok);
			if (left == nullptr) {
				std::cerr << "\033[31munnamed error in Solver::build(bool)\n";
				std::cerr << "the token that gave the error" << tok << "\033[0m\n";
				exit (1);
			}
		}
		while ( !EOE() ) {
			
			// get the tokens from right to left
			token tok = eat();
			

			// now take care of the root
			AST_Node* operation = new AST_Node ( tok.type ); // temporary
			
			AST_Node* right = nullptr; // the right value for the operant
			if ( EOE() ) {
				std::cerr << "no num error \n";
				exit (1);
			}
			token rtok = eat();
			right = getOPNum(rtok);
			if (right == nullptr) {
				std::cerr << "unnamed error in Solver::build(bool) for right\n";
				exit (1);
			}
			// setting up this operations values
			operation->left = left;
			left = nullptr;
			operation->right = right;
			right = nullptr;
			// the different actions to take for the different situatuations
			if (previous_operator_priority == -1) {
				root = operation;
				previous_operator_priority = tok.priority;
				continue;
				return root;
			} else if (previous_operator_priority == tok.priority) {
				operation->left = root;
				root = operation;
				previous_operator_priority = tok.priority;
				continue;
			}


		}
		
		return root;
	}

	void Solver::printTree () {
		printAST (root);
	}

};


int main () {
	PMES::Solver solver("1+2+3+4+5");
	solver.lex();
	solver.printTokens();
	solver.build();
	solver.printTree ();
	return 0;
}
