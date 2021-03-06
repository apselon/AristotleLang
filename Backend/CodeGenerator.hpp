#pragma once
#include "../Lib/CompLib.hpp"
#include "../Frontend/ASTree.cpp"

namespace CodeGeneratorNS {
	class CodeGenerator {
	private:
		Vector<Assembly::Instruction*> instructions;

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
		void generate_print(ASTreeNS::ASTNode_t* node);

		HashTable<const char*, int32_t, hash, strcmp, 509>* local_offsets = nullptr;
		size_t cur_local_vars_num = 0;
		size_t cur_local_args_num = 0;

		size_t num_blocks = 0;

		void count_local_offsets(ASTreeNS::ASTNode_t* node, size_t& num_vars);
		void count_function_arguments(ASTreeNS::ASTNode_t* node, size_t& num_args);

	public:
		CodeGenerator(const ASTreeNS::ASTree& tree);

		void write_asm(const char* filename);
		void write_asm(FILE* output_f);

		size_t write_elf(uint8_t* buf);
			
	};
};
