#include <iostream>
#include <vector>
#include <string>
#include <inttypes.h>
#include <math.h>

#ifndef  PMES__
#define  PMES__

namespace PMES {

	typedef long double VALUE;

  // some error const that isVlalid returns
  const uint8_t INVALID_CHARACTER     = 0b00000001;
  const uint8_t INVALID_TOO_MANY_PC   = 0b00000010;
  const uint8_t INVALID_TOO_MANY_PO   = 0b00000100;
  const uint8_t INVALID_OP_AFTER_OP   = 0b00001000;
  const uint8_t INVALID_NUM_AFTER_NUM = 0b00010000;
  const uint8_t INVALID_PO_AFTER_NUM  = 0b00100000;
  const uint8_t INVALID_PC_AFTER_OP   = 0b01000000;
  const uint8_t INVALID_LAST_IS_OP    = 0b10000000;


	enum class TokenType {
		NUL=0,
		
		NUMBER,
		PARENT_OPENING,
		PARENT_CLOSING,

		OP_ADD,
		OP_SUB,
		OP_MUL,
		OP_DIV,
		OP_POW
	};


	struct token {
		TokenType type;
		VALUE value;
		uint8_t priority;

		token (TokenType type, uint8_t priority, VALUE value=0) {
			this->type = type;
			this->priority = priority;
			this->value = value;
		}
	};

	struct AST_Node {
		VALUE value;
		TokenType type;

		AST_Node* left;
		AST_Node* right;

		AST_Node (TokenType type , VALUE value=0) {
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
	void deleteAST (AST_Node*);

	class Solver {
	public:
		Solver ();
		~Solver();
		void lex (std::string);// will change the string so not a const std::string:string&
		void lex (); // cant pass default arguments so this
		void build(); // wrapper for private build
		VALUE solve (); // 
		VALUE parse (const std::string&); 
		void printTokens ();
		void printTree ();
	  uint32_t isValid (const std::string&);
		void clear();
		//void clearTokens();
	private:
		std::vector <token> tokens;
		std::string srcString;
		AST_Node* root;
	
		VALUE solve (AST_Node*);
		AST_Node* getOPNum (const token&);
		AST_Node* build(bool);
		VALUE getNumber (const std::string& , uint32_t*);
		token eat (TokenType=TokenType::NUL);
		bool EOE();// stands for end of expression
	};

};

#endif
