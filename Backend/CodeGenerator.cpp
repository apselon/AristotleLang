#include "CodeGenerator.hpp"

namespace CodeGeneratorNS {
	CodeGenerator::CodeGenerator(const ASTreeNS::ASTree& tree){
		cur	= tree.root();
		generate_body(cur->right());
		
	}

	void CodeGenerator::generate_operator(ASTreeNS::ASTNode_t* node){
		assert(node != nullptr);

		switch (node->key.code){
		case Operator::IF:
		//	generate_branching(node);
			break;
		case Operator::DEC_VAR:
//			generate_var_declaration(node);
			break;
		case Operator::ASSGN:
			generate_var_init(node);
			break;
		case Operator::RETURN:
		//	generate_return(node);
			break;
		case Operator::DEC_FUNC:
		//	generate_func_declaration(node);
			break;
		case Operator::WRITE:
		//	generate_print(node);
			break;
		case Operator::EXIT:
		//	generate_exit(node);
			break;
		case Operator::READ:
		//	generate_input(node);
			break;
		case Operator::SQRT:
		//	generate_sqrt(node);
			break;
		default:
		//	generate_nop();
			printf("Wrong operator %s\n", node->key.lexem);
			break;
		}
	}

	void CodeGenerator::generate_body(ASTreeNS::ASTNode_t* node){
		assert(node != nullptr);

		cur_local_vars_num = 0;
		local_offsets = new std::unordered_map<const char*, int64_t>();

		instructions.push_back(new Assembly::Label(num_blocks++));
		instructions.push_back(new Assembly::PushReg(Assembly::Registers::RBP));
		instructions.push_back(new Assembly::MovReg2Reg(Assembly::Registers::RBP, Assembly::Registers::RSP));

		count_local_vars(node, cur_local_vars_num);

		instructions.push_back(new Assembly::SubReg2Val(Assembly::Registers::RSP, cur_local_vars_num * 8));

		while (node != nullptr && node->right() != nullptr){
			generate_operator(node->right());

			node = node->left();
		}

		instructions.push_back(new Assembly::MovReg2Reg(Assembly::Registers::RSP, Assembly::Registers::RBP));
		instructions.push_back(new Assembly::PopReg(Assembly::Registers::RBP));
		instructions.push_back(new Assembly::Ret());
	}

	void CodeGenerator::generate_expression(ASTreeNS::ASTNode_t* node){
		assert(node != nullptr);

		if (node->left () != nullptr){
			generate_expression(node->left ());
		}

		if (node->right() != nullptr){
			instructions.push_back(new Assembly::PushReg(Assembly::Registers::R10));
			generate_expression(node->right());
			instructions.push_back(new Assembly::PopReg(Assembly::Registers::R11));
		}

		if (node->key.type == TokenizerNS::NUM){
			instructions.push_back(new Assembly::MovVal2Reg(Assembly::Registers::R10,
						                                    atoi(node->key.lexem)));
		}

		if (node->key.type == TokenizerNS::ID){
			instructions.push_back(new Assembly::MovMem2Reg(Assembly::Registers::R10, Assembly::Registers::RBP,
			                                                local_offsets->at(node->key.lexem)));
		}

		if (node->key.type == TokenizerNS::OP && node->key.code != Operator::COMMA){
			switch (node->key.code){
				case Operator::ADD:
					instructions.push_back(new Assembly::AddReg2Reg(Assembly::Registers::R10,
					                                                Assembly::Registers::R11));
					break;

				case Operator::SUB:
					instructions.push_back(new Assembly::SubReg2Reg(Assembly::Registers::R10,
					                                                Assembly::Registers::R11));
					break;

				case Operator::MUL:
					instructions.push_back(new Assembly::MulReg2Reg(Assembly::Registers::R10,
					                                                Assembly::Registers::R11));
					break;

				case Operator::DIV:
					instructions.push_back(new Assembly::DivReg2Reg(Assembly::Registers::R10,
					                                                Assembly::Registers::R11));
					break;
				
				default:
					assert("Unknown operator code" && false);
			}
		}
	}

	void CodeGenerator::count_local_vars(ASTreeNS::ASTNode_t* node, size_t& num_vars){
		assert(node != nullptr);

		if (node->key.type == TokenizerNS::ID){
			++num_vars;
			local_offsets->emplace(node->key.lexem, num_vars * (-8));
		}

		if (node->left() != nullptr){
			count_local_vars(node->left(), num_vars);
		}

		if (node->right() != nullptr){
			count_local_vars(node->right(), num_vars);
		}
	}

	void CodeGenerator::generate_var_init(ASTreeNS::ASTNode_t* node){
		assert(node != nullptr);
		generate_expression(node->right());
		instructions.push_back(new Assembly::MovReg2Mem(Assembly::Registers::RBP,
		                       local_offsets->at(node->left()->key.lexem), Assembly::Registers::R10));

	}

	void CodeGenerator::write(FILE* output_f){
		assert(output_f != nullptr);

		for (auto i = instructions.begin(); i != instructions.end(); ++i){
			fprintf(output_f, "%s\n", (*i)->assembly());
		}
	}

	void CodeGenerator::write(const char* filename){
		assert(filename != nullptr);

		FILE* output_f = fopen(filename, "w");
		write(output_f);
		fclose(output_f);
	}
}
