#include "MathExpressionSolver.h"


namespace PMES  {

	
	std::ostream& operator << (std::ostream& stream , TokenType tok) {
		switch (tok) {
			case TokenType::NUMBER:
				stream << "Number type";
				break;
			case TokenType::NUL:
				stream << "Null type";
				break;
			case TokenType::PARENT_OPENING:
				stream << "parentheses opening type";
				break;
			case TokenType::PARENT_CLOSING:
				stream << "parentheses closing type";
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
			case TokenType::OP_POW:
				stream << "power operation";
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

	void deleteAST (AST_Node* node) {
		if (node == nullptr)
			return;
		if (node->type == TokenType::NUMBER){
			delete node;
			node = nullptr;
			return;
		}

		deleteAST (node->left);
		deleteAST (node->right);
		delete node;
		node = nullptr;
	}
	

	Solver::Solver () {
	  root = nullptr;
  }
	Solver::~Solver () {
		deleteAST (root);
	}

	PMES::VALUE Solver::getNumber (const std::string& src , uint32_t* ix) {
		std::string temp;

		uint32_t& index = (*ix);

		while ( index < src.size() && (isdigit(src.at(index)) || src.at(index) == '.') ) {
			temp += src.at(index);
			++(index);
		}
		//--(index);
		return stold (temp);
	
	}

	
	void Solver::lex () {
		this->lex (srcString);
	}
	void Solver::lex (std::string src) {
		uint32_t index = 0;
		bool lastIsNumber = false;
		// remove all spaces cause they suck
		src.erase ( std::remove(src.begin() , src.end() , ' ') , src.end() );
		
		while ( index < src.size() ) {
			if ( isdigit( src.at(index) ) ){
				lastIsNumber = true;
				PMES::VALUE num = getNumber (src , &index);
				tokens.push_back (token(TokenType::NUMBER , 0 , num));
				continue;
			}
			if (((index+1) < src.size()) && (src.at(index) == '-') && (isdigit( src.at(index+1))) && (!lastIsNumber) ){
				lastIsNumber = true;
				++index;
				PMES::VALUE num = getNumber (src , &index);
				tokens.push_back (token(TokenType::NUMBER , 0 , num*-1));
				continue;
			}
			lastIsNumber = false;

			switch ( src.at(index) ) {
				case '+':
					tokens.push_back (token(TokenType::OP_ADD, 1));
					break;
				case '-':
					tokens.push_back (token(TokenType::OP_SUB, 1));
					break;
				case '*':
					tokens.push_back (token(TokenType::OP_MUL, 2));
					break;
				case '/':
					tokens.push_back (token(TokenType::OP_DIV, 2));
					break;
				case '^':
					tokens.push_back (token(TokenType::OP_POW, 3));
					break;
				case '(':
					tokens.push_back (token(TokenType::PARENT_OPENING, 4));
					break;
				case ')':
					tokens.push_back (token(TokenType::PARENT_CLOSING, 4));
					break;
				case ' ':
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
		if (tokens.size() == 0)
			return nullptr;
		AST_Node* root = nullptr;
		AST_Node* left = nullptr;
		int16_t previous_operator_priority = -1; // has the last operators priority , -1 stands for the first iteration
		int16_t root_priority = -1;

		// if the left node is empty configure it
		token tok = eat();
		left = getOPNum(tok);
		if (left == nullptr) {
			std::cerr << "\033[31munnamed error in Solver::build(bool)\n";
			std::cerr << "the token that gave the error" << tok << "\033[0m\n";
			exit (1);
		}
		if (tokens.size() == 0)
			return left;
		tok = eat();
		root = new AST_Node(tok.type);
		root->left = left;
		previous_operator_priority = tok.priority;
		root_priority = tok.priority;
		tok = eat();// left is used for right
		AST_Node* right = getOPNum(tok); // the right value for the operant
		if (right == nullptr) {
			std::cerr << "\033[31munnamed error in Solver::build(bool) for right\n";
			std::cerr << "the token that gave the error" << tok << "\033[0m\n";
			exit (1);
		}
		root->right = right;
		left = right;// the next operatiors left is this operators right
		if (tokens.size() == 0)
			return root;
			
		while ( !EOE() ) {
			
			// get the tokens from right to left
			tok = eat();

			if (tok.type == TokenType::PARENT_CLOSING) {
				if (openParent)
					return root;
				else {
					std::cout << "err\n";
					exit (1);
				}
			}
			

			// now take care of the root
			AST_Node* operation = new AST_Node ( tok.type ); // temporary
			
			//AST_Node* right = nullptr; // the right value for the operant
			if ( EOE() ) {
				std::cerr << "no num error \n";
				exit (1);
			}
			token rtok = eat();
			right = getOPNum(rtok);
			if (right == nullptr) {
				std::cerr << "\033[31munnamed error in Solver::build(bool) for right\n";
				std::cerr << "the token that gave the error" << tok << "\033[0m\n";
				exit (1);
			}
			// setting up this operations values
			operation->right = right;
			operation->left = left;
			// the different actions to take for the different situatuations
			if (previous_operator_priority == -1) {
				std::cout << "err\n";
				root = operation;
				previous_operator_priority = tok.priority;
			} else if (previous_operator_priority == tok.priority) {
				if (root_priority == tok.priority) { // might have to tinker with this
					operation->left = root;
					root = operation;
				} else if (root_priority < tok.priority) {
					operation->left = root->right;
					root->right = operation;
				} else if (root_priority > tok.priority) {
					operation->left = root;
					root = operation;
				}
			
			}else if (previous_operator_priority < tok.priority) {
				// 5 + 6 * 3
				root->right = operation;

			}else if (previous_operator_priority > tok.priority) {
				operation->left = root;
				root = operation;
			}

			previous_operator_priority = tok.priority;
			left = right;
			right = nullptr;

		}
		
		return root;
	}

	void Solver::printTree () {
		printAST (root);
	}


	PMES::VALUE Solver::solve (AST_Node* root) {
		if (root->type == TokenType::NUMBER)
			return root->value;
		PMES::VALUE num1 = solve (root->left);
		PMES::VALUE num2 = solve (root->right);

		switch (root->type) {
			case TokenType::NUMBER:
			case TokenType::PARENT_OPENING:
			case TokenType::PARENT_CLOSING:
			case TokenType::NUL:
				std::cerr << "\033[31m illegale token type in solve encountered\033[0m\n";
			case TokenType::OP_ADD :
				return num1 + num2;
			case TokenType::OP_SUB :
				return num1 - num2;
			case TokenType::OP_MUL :
				return num1 * num2;
			case TokenType::OP_DIV :
				return num1 / num2;
			case TokenType::OP_POW :
				return pow( num1 , num2 );
		}
		std::cerr << "\033[31m unknown token in PMES::VALUE Solver::solve\033[0m\n";
		return 0;
	
	}
	PMES::VALUE Solver::solve () {
		return solve (root);
	}

	PMES::VALUE Solver::parse (const std::string& src) {
		tokens.clear();
		this->lex (src);
		this->build();
		return this->solve ();
	}

	uint32_t Solver::isValid (const std::string& str) {
		uint32_t err = 0;

		this->clear();
		this->lex(str);

		bool lastIsOperator = false;
		uint32_t openParents = 0;
		for (uint32_t i = 0; i < tokens.size();++i) {
			if ( tokens.at(i).type == TokenType::NUMBER ) {
				if (!lastIsOperator)
					err |= INVALID_NUM_AFTER_NUM;
				lastIsOperator = false;
			}else if ( tokens.at(i).type == TokenType::PARENT_OPENING ){
				++openParents;
				if (!lastIsOperator && i != 0)
					err |= INVALID_NUM_AFTER_NUM;
				lastIsOperator = true;
			
			}else if ( tokens.at(i).type == TokenType::PARENT_CLOSING ) {
				if (openParents > 0)
					--openParents;
				else {
					err |= INVALID_TOO_MANY_PC;
					--openParents;
				}
			}else  {
				if (lastIsOperator)
					err |= INVALID_OP_AFTER_OP;
				lastIsOperator = true;
			}
		}

		if ( openParents > 0 )
			err |= INVALID_TOO_MANY_PO;

		return err;
	}

	void Solver::clear () {
		tokens.clear();
		srcString.clear();
		deleteAST (root);
	}


};
