#pragma once

#include "../Lib/CompLib.hpp"
#include "../Lib/Tree.cpp"
#include "../Tokenizer/Tokenizer.cpp"

namespace ASTreeNS {
	using ASTNode_t = TNode_t<TokenizerNS::Token>;
	
	class ASTree {
	private:
		ASTNode_t* root_ = new ASTNode_t(TokenizerNS::Token(";_START", TokenizerNS::ID, Operator::BLOCK));
		TokenizerNS::Token* cur_token = nullptr;
//		ASTNode_t* (parsers[30])(void){};
		
	public:
		ASTree(TokenizerNS::Token*);
		~ASTree();
		ASTNode_t root();

		void traverse_nodes(void (*action)(ASTNode_t*));
		void traverse_nodes(void (*action)(ASTNode_t*), ASTNode_t* cur);
		void list_nodes(ASTNode_t* cur_node, FILE* out);

		void dump(const char* filename);
		
		ASTNode_t* parse_block();
		ASTNode_t* parse_operator();
		ASTNode_t* parse_if();
		ASTNode_t* parse_while();
		ASTNode_t* parse_var_decl();
		ASTNode_t* parse_func_decl();
		ASTNode_t* parse_var_init();
		ASTNode_t* parse_func_call();
		ASTNode_t* parse_return();
		ASTNode_t* parse_print();
		ASTNode_t* parse_sqrt();
		ASTNode_t* parse_exit();
		ASTNode_t* parse_input();
		ASTNode_t* parse_expression();
		ASTNode_t* parse_id();
		ASTNode_t* parse_varlist();
		ASTNode_t* parse_multiplication();
		ASTNode_t* parse_brackets();
		ASTNode_t* parse_number();

		
	};
};
