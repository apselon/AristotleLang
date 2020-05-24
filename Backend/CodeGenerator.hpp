#pragma once
#include "../Lib/CompLib.hpp"
#include "../Frontend/ASTree.cpp"

namespace CodeGeneratorNS {
	class CodeGenerator {
	private:
		std::vector<Assembly::Instruction*> instructions;

		ASTreeNS::ASTNode_t* cur = nullptr;

		void generate_body(ASTreeNS::ASTNode_t* node);
		void generate_operator(ASTreeNS::ASTNode_t* node);
		void generate_var_declaration(ASTreeNS::ASTNode_t* node);
		void generate_expression(ASTreeNS::ASTNode_t* node);
		void generate_var_init(ASTreeNS::ASTNode_t* node);
		void generate_func_declaration(ASTreeNS::ASTNode_t* node);
		void generate_block(ASTreeNS::ASTNode_t* node);
		void generate_return(ASTreeNS::ASTNode_t* node);
		void push_arguments(ASTreeNS::ASTNode_t* node, size_t& num_args);
		void generate_branching(ASTreeNS::ASTNode_t* node);
		void generate_exit(ASTreeNS::ASTNode_t* node);

		/*
		void generate_condition(ASTreeNS::ASTNode_t* node);
		void generate_print(ASTreeNS::ASTNode_t* node);
		void generate_sqrt(ASTreeNS::ASTNode_t* node);
		void generate_input(ASTreeNS::ASTNode_t* node);
		void generate_nop();
		*/

		std::map<const char*, int64_t, cmp_str>* local_offsets = nullptr;
		size_t cur_local_vars_num = 0;
		size_t cur_local_args_num = 0;

		size_t num_blocks = 0;

		void count_local_vars(ASTreeNS::ASTNode_t* node, size_t& num_vars);
		void count_function_arguments(ASTreeNS::ASTNode_t* node, size_t& num_args);

	public:
		CodeGenerator(const ASTreeNS::ASTree& tree);

		void write(const char* filename);
		void write(FILE* output_f);
			
	};
};
