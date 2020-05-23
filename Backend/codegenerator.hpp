#pragma once
#include "../Lib/CompLib.hpp"
#include "../Frontend/ASTree.cpp"

namespace CodeGeneratorNS {
	class CodeGenerator {
	private:
		char* text = new char[8192]();	
		char* data = new char[1024]();

		ASTreeNS::ASTNode_t* cur = nullptr;

		void generate_body(ASTreeNS::ASTNode_t* node);
		void generate_operator(ASTreeNS::ASTNode_t* node);
		void generate_var_declaration(ASTreeNS::ASTNode_t* node);
		void generate_return(ASTreeNS::ASTNode_t* node);
		void generate_expression(ASTreeNS::ASTNode_t* node);
		void generate_condition(ASTreeNS::ASTNode_t* node);
		void generate_call(ASTreeNS::ASTNode_t* node);
		void generate_var_init(ASTreeNS::ASTNode_t* node);
		void generate_branching(ASTreeNS::ASTNode_t* node);
		void generate_print(ASTreeNS::ASTNode_t* node);
		void generate_exit(ASTreeNS::ASTNode_t* node);
		void generate_sqrt(ASTreeNS::ASTNode_t* node);
		void generate_input(ASTreeNS::ASTNode_t* node);
		void generate_func_declaration(ASTreeNS::ASTNode_t* node);
		void generate_nop();

	public:
		CodeGenerator(const ASTreeNS::ASTree& tree);

		void write(const char* filename);
		void write(FILE* output_f);
			
	};
};
