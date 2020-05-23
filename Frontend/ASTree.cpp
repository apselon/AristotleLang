#pragma once
#include "ASTree.hpp"

namespace ASTreeNS {
	ASTree::ASTree(TokenizerNS::Token* cur_token): cur_token(cur_token){
		root_->attach_right(parse_block());
	}

	ASTree::~ASTree(){
		root_->remove_subtree();
	}

	ASTNode_t* ASTree::root() const {
		return root_;
	}

	void ASTree::list_nodes(ASTNode_t* cur_node, FILE* out){

    	assert(cur_node != NULL);

		if (cur_node->left() != nullptr){
              list_nodes(cur_node->left(), out);
		}

		if (cur_node->right() != nullptr){
              list_nodes(cur_node->right(), out);
		}

		fprintf(out, "\t\"%p\" [label = \"{%p |%s |%s}\"]\n", cur_node, cur_node, cur_node->key.lexem, 
		        (cur_node->key.code >= 0)?(Operator::op_code_strs[cur_node->key.code]):("NOT_OP"));
    }

	void generate_arrows(ASTNode_t* cur_node, FILE* out){

		assert(cur_node != NULL);

        fprintf(out, "\tedge [color = \"#19A302\"] ;\n");
        if (cur_node->left() != nullptr){
              fprintf(out, "\t\"%p\" -> \"%p\";\n", cur_node, cur_node->left());
              generate_arrows(cur_node->left(), out);
          }

        fprintf(out, "\tedge [color = \"#C00303\"] ;\n");
        if (cur_node->right() != nullptr){
        	fprintf(out, "\t\"%p\" -> \"%p\";\n", cur_node, cur_node->right());
            generate_arrows(cur_node->right(), out);
        }
	}


	void ASTree::traverse_nodes(void (*action)(ASTNode_t*), ASTNode_t* cur){
		assert(cur != nullptr);

		if (cur->left() != nullptr){
			traverse_nodes(action, cur->left());
		}

		if (cur->right() != nullptr){
			traverse_nodes(action, cur->right());
		}

		action(cur);
	}

	void ASTree::dump(const char* filename){
		assert(filename != nullptr);

		FILE* dump_f = fopen(filename, "w");

		fprintf(dump_f, "digraph List {\n");
		fprintf(dump_f, "\tnode [shape=\"record\", fontsize=15] ;\n");
		fprintf(dump_f, "\trankdir=TB;\n");

		list_nodes(root_, dump_f);
		generate_arrows(root_, dump_f);

        fprintf(dump_f, "}");

        fclose(dump_f);
	}
	
	ASTNode_t* ASTree::parse_block(){

		assert(cur_token->code == Operator::O_BRACK);
		++cur_token;

		ASTNode_t* block = new ASTNode_t(SPEC_BLOCK);
		ASTNode_t* new_block = nullptr;
		ASTNode_t* val = block;

		while (cur_token->code != Operator::C_BRACK){
			block->attach_right(parse_operator());
			if (cur_token->code == Operator::C_BRACK) break;

			new_block = new ASTNode_t(SPEC_BLOCK);
			block->attach_left(new_block);

			block = new_block;
		}

		++cur_token;
		return val;
	}

	ASTNode_t* ASTree::parse_operator(){

		ASTNode_t* val = nullptr; 

		switch (cur_token->code){
			case Operator::IF:
				val = parse_if();
				break;
			case Operator::WHILE:
				val = parse_while();
				break;
			case Operator::DEC_VAR:
				val = parse_var_decl();
				break;
			case Operator::DEC_FUNC:
				val = parse_func_decl();
				break;
			case Operator::ASSGN:
				val = parse_var_init();
				break;
			case Operator::CALL:
				val = parse_func_call();
				break;
			case Operator::RETURN:
				val = parse_return();
				break;
			case Operator::WRITE:
				val = parse_print();
				break;
			case Operator::SQRT:
				val = parse_sqrt();
				break;
			case Operator::EXIT:
				val = parse_exit();
				break;
			case Operator::READ:
				val = parse_input();
				break;
			default:
				printf("OP %d (%s) can not be parsed\n", cur_token->code, Operator::op_code_strs[cur_token->code]);
				break;
		}

		return val;
	}

	ASTNode_t* ASTree::parse_if(){
			assert(cur_token->code == Operator::IF);
			++cur_token;

			ASTNode_t* cond = parse_expression();
			ASTNode_t* main_body = parse_block();
			ASTNode_t* else_body = parse_block();

			ASTNode_t* connection = new ASTNode_t(SPEC_CONNECTION, else_body, main_body);


			return new ASTNode_t(TokenizerNS::Token("IF", TokenizerNS::OP, Operator::IF), cond, connection);
	}

	ASTNode_t* ASTree::parse_while(){
		return nullptr;
	}

	ASTNode_t* ASTree::parse_var_decl(){
		assert(cur_token->code == Operator::DEC_VAR);
		++cur_token;

		ASTNode_t* var = parse_id();

		return new ASTNode_t(TokenizerNS::Token("DEC_VAR", TokenizerNS::SPEC, Operator::DEC_VAR), nullptr, var);
	}

	ASTNode_t* ASTree::parse_func_decl(){
		assert(cur_token->code == Operator::DEC_FUNC);
		++cur_token;

		ASTNode_t* name = parse_id();
		ASTNode_t* args = parse_varlist();
		ASTNode_t* body = parse_block();

		name->attach_right(body);

		return new ASTNode_t(TokenizerNS::Token("DEF_FUNC", TokenizerNS::SPEC, Operator::DEC_FUNC), args, name);
	}

	ASTNode_t* ASTree::parse_var_init(){
		assert(cur_token->code == Operator::ASSGN);
		++cur_token;

		ASTNode_t* var = parse_id();
		ASTNode_t* val = nullptr;

		if (cur_token->code == Operator::CALL){
			val = parse_func_call();
		}
		
		else {
			val = parse_expression();
		}

		return new ASTNode_t(TokenizerNS::Token("=", TokenizerNS::SPEC, Operator::ASSGN), var, val);
	}

	ASTNode_t* ASTree::parse_func_call(){
		assert(cur_token->code == Operator::CALL);
		++cur_token;

		ASTNode_t* name = parse_id();
		ASTNode_t* args = parse_varlist();

		return new ASTNode_t(TokenizerNS::Token("CALL", TokenizerNS::OP, Operator::CALL), args, name);
	}

	ASTNode_t* ASTree::parse_return(){
		assert(cur_token->code == Operator::RETURN);
		++cur_token;

		ASTNode_t* val = parse_expression();

		return new ASTNode_t(TokenizerNS::Token("RET", TokenizerNS::OP, Operator::RETURN), nullptr, val);
	}

	ASTNode_t* ASTree::parse_print(){
		assert(cur_token->code == Operator::WRITE);
		++cur_token;

		ASTNode_t* val = parse_expression();

		return new ASTNode_t(TokenizerNS::Token("OUT", TokenizerNS::OP, Operator::WRITE), nullptr, val);
	}

	ASTNode_t* ASTree::parse_sqrt(){
		assert(cur_token->code == Operator::SQRT);
		++cur_token;

		ASTNode_t* val = parse_expression();

		return new ASTNode_t(TokenizerNS::Token("SQRT", TokenizerNS::OP, Operator::SQRT), nullptr, val);
	}

	ASTNode_t* ASTree::parse_exit(){
		assert(cur_token->code == Operator::EXIT);
		++cur_token;

		return new ASTNode_t(TokenizerNS::Token("EXIT", TokenizerNS::OP, Operator::EXIT));
	}

	ASTNode_t* ASTree::parse_input(){
		assert(cur_token->code == Operator::READ);
		++cur_token;

		ASTNode_t* var = parse_id();

		return new ASTNode_t(TokenizerNS::Token("IN", TokenizerNS::OP, Operator::READ), nullptr, var);
	}

	ASTNode_t* ASTree::parse_expression(){
		
		ASTNode_t* val1 = parse_multiplication();
		TokenizerNS::Token* sign = cur_token;
		ASTNode_t* val2 = nullptr;

		while (cur_token->code == Operator::ADD  || cur_token->code == Operator::SUB    ||
		       cur_token->code == Operator::EQL  || cur_token->code == Operator::LESS   ||
		       cur_token->code == Operator::MORE || cur_token->code == Operator::EQLESS || cur_token->code == Operator::EQMORE){

			sign = cur_token++;
			val2 = parse_multiplication();
			val1 = new ASTNode_t(*sign, val1, val2);

		}

		return val1;
	}

	ASTNode_t* ASTree::parse_id(){
		assert(cur_token->type == TokenizerNS::ID);

		return new ASTNode_t(*(cur_token++));
	}

	ASTNode_t* ASTree::parse_varlist(){
		assert(cur_token->code == Operator::O_BRACK);		
		++cur_token;

		ASTNode_t* comma = new ASTNode_t(TokenizerNS::Token("VARLIST", TokenizerNS::OP, Operator::COMMA));
		ASTNode_t* next_comma = nullptr;
		ASTNode_t* val = comma;

		while (cur_token->code != Operator::C_BRACK){
			comma->attach_right(parse_expression());
			if (cur_token->code == Operator::C_BRACK) break;

			next_comma = new ASTNode_t(TokenizerNS::Token("VARLIST", TokenizerNS::OP, Operator::COMMA));
			comma->attach_left(next_comma);

			comma = next_comma;
		}

		++cur_token;
		return val;
	}

	ASTNode_t* ASTree::parse_multiplication(){

		ASTNode_t* val1 = parse_brackets();
		ASTNode_t* val2 = nullptr;
		TokenizerNS::Token* sign = nullptr;

		while (cur_token->code == Operator::MUL || cur_token->code == Operator::DIV){
			sign = cur_token++;
			val2 = parse_brackets();
			val1 = new ASTNode_t(*sign, val1, val2);
		}

		return val1;
	}

	ASTNode_t* ASTree::parse_brackets(){

		ASTNode_t* val = nullptr;
		
		if (cur_token->code == Operator::O_BRACK){
			++cur_token;
			val = parse_expression();
			assert(cur_token->code == Operator::C_BRACK);
			++cur_token;

			return val;
		}

		if (cur_token->type == TokenizerNS::ID){
			return parse_id();
		}

		return parse_number();
	}

	ASTNode_t* ASTree::parse_number(){
		assert(cur_token->type == TokenizerNS::NUM);
		return new ASTNode_t(*(cur_token++));
	}
};
