#include <iostream>
#include <vector>
#include <string>
#include <inttypes.h>

#ifndef  PMES__
#define  PMES__

namespace PMES {

	enum class TokenType {
		NUL=0,
		
		NUMBER,
		PARENT_OPENING,
		PARENT_CLOSING,

		OP_ADD,
		OP_SUB,
		OP_MUL,
		OP_DIV
	};


	struct token {
		TokenType type;
		int64_t value;
		uint8_t priority;

		token (TokenType type, uint8_t priority, int64_t value=0) {
			this->type = type;
			this->priority = priority;
			this->value = value;
		}
	};

	struct AST_Node {
		int64_t value;
		TokenType type;

		AST_Node* left;
		AST_Node* right;

		AST_Node (TokenType type , int64_t value=0) {
			this->type = type;
			this->value = value;
		}
		AST_Node () {
		}
	
	};

	std::ostream& operator << (std::ostream& , TokenType);
	std::string TokenTypeToSRC (TokenType);

	std::ostream& operator << (std::ostream& , const token&);

	void printTabs (uint8_t);
	void printAST (AST_Node*,uint8_t=0);

	class Solver {
	public:
		Solver (std::string);
		~Solver();
		void lex (const std::string&);
		void lex (); // cant pass default arguments so this
		void build(); // wrapper for private build
		int64_t solve (); // 
		void printTokens ();
		void printTree ();
	private:
		std::vector <token> tokens;
		std::string srcString;
		AST_Node* root;
	
		int64_t solve (AST_Node*);
		AST_Node* getOPNum (const token&);
		AST_Node* build(bool);
		int64_t getNumber (const std::string& , uint32_t*);
		token eat (TokenType=TokenType::NUL);
		bool EOE();// stands for end of expression
	};

};

#endif
