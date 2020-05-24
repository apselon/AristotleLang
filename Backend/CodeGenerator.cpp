#include "CodeGenerator.hpp"

namespace CodeGeneratorNS {
	CodeGenerator::CodeGenerator(const ASTreeNS::ASTree& tree){
		cur	= tree.root();
		instructions.push_back(new Assembly::Section(".text"));
		instructions.push_back(new Assembly::Array("num_format", "'%d', 10d, 0"));
		instructions.push_back(new Assembly::Extern("_vprintf"));
		instructions.push_back(new Assembly::Global("_start"));
		generate_block(cur->right());
		
	}

	void CodeGenerator::generate_operator(ASTreeNS::ASTNode_t* node){
		assert(node != nullptr);

		switch (node->key.code){
		case Operator::IF:
			generate_branching(node);
			break;
		case Operator::DEC_VAR:
			break;
		case Operator::ASSGN:
			generate_var_init(node);
			break;
		case Operator::RETURN:
			generate_return(node);
			break;
		case Operator::DEC_FUNC:
			generate_func_declaration(node);
			break;
		case Operator::WRITE:
			generate_print(node);
			break;
		case Operator::EXIT:
			generate_exit(node);
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

	void CodeGenerator::generate_block(ASTreeNS::ASTNode_t* node){
		assert(node != nullptr);
		assert(node->key.code == Operator::BLOCK);

		size_t cur_bloc_num = num_blocks++;
		instructions.push_back(new Assembly::Label(cur_bloc_num));

		while (node != nullptr && node->right() != nullptr){
			generate_operator(node->right());

			node = node->left();
		}

		instructions.push_back(new Assembly::Comment(cur_bloc_num));
	}

	void CodeGenerator::generate_func_declaration(ASTreeNS::ASTNode_t* node){
		assert(node != nullptr);
		assert(node->key.code == Operator::DEC_FUNC);

		cur_local_vars_num = 0;
		cur_local_args_num = 0;

		local_offsets = new std::map<const char*, int64_t, cmp_str>();

		instructions.push_back(new Assembly::Label(node->right()->key.lexem));
		instructions.push_back(new Assembly::Comment("{"));
		instructions.push_back(new Assembly::PushReg(Assembly::Registers::RBP));
		instructions.push_back(new Assembly::MovReg2Reg(Assembly::Registers::RBP, Assembly::Registers::RSP));

		count_local_vars(node->right(), cur_local_vars_num);
		count_function_arguments(node->left(), cur_local_args_num);

		generate_block(node->right()->right());

		instructions.push_back(new Assembly::Comment("}"));
	}

	void CodeGenerator::generate_expression(ASTreeNS::ASTNode_t* node){
		assert(node != nullptr);

		if (node->right() != nullptr){
			generate_expression(node->right());
		}

		if (node->left() != nullptr){
			instructions.push_back(new Assembly::PushReg(Assembly::Registers::R10));
			generate_expression(node->left());
			instructions.push_back(new Assembly::PopReg(Assembly::Registers::R11));
		}

		if (node->key.type == TokenizerNS::NUM){
			instructions.push_back(new Assembly::MovVal2Reg(Assembly::Registers::R10,
						                                    atoi(node->key.lexem)));
		}

		if (node->key.type == TokenizerNS::ID && node->parent()->key.code != Operator::CALL){

			try{
				instructions.push_back(new Assembly::MovMem2Reg(Assembly::Registers::R10, Assembly::Registers::RBP,
			                                                local_offsets->at(node->key.lexem)));
			
			} catch (...){
				asm("int $3");
				std::cout << node->key.lexem << "!!!\n" ;
			}
		}

		if (node->key.type == TokenizerNS::OP && node->key.code != Operator::COMMA){
			size_t cur_num_args = 0;

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

				case Operator::CALL:
					push_arguments(node->left(), cur_num_args);
					instructions.push_back(new Assembly::Call(node->right()->key.lexem));
					instructions.push_back(new Assembly::AddVal2Reg(Assembly::Registers::RSP, cur_num_args * 8));
					break;
				
				default:
					assert("Unknown operator code" && false);
			}
		}
	}

	void CodeGenerator::push_arguments(ASTreeNS::ASTNode_t* node, size_t& num_args){
		assert(node != nullptr);
		assert(node->key.code == Operator::COMMA);
		
		if (node->right() != nullptr){
			++num_args;
			generate_expression(node->right());
			instructions.push_back(new Assembly::PushReg(Assembly::Registers::R10));

			if (node->left() != nullptr){
				push_arguments(node->left(), num_args);
			}
		}

	}

	void CodeGenerator::count_local_vars(ASTreeNS::ASTNode_t* node, size_t& num_vars){
		assert(node != nullptr);

		if (node->key.code == Operator::DEC_VAR){
			++num_vars;
			local_offsets->emplace(node->right()->key.lexem, num_vars * (-8));
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
		assert(node->key.code == Operator::ASSGN);

		generate_expression(node->right());

		instructions.push_back(new Assembly::MovReg2Mem(Assembly::Registers::RBP,
		                       local_offsets->at(node->left()->key.lexem), Assembly::Registers::R10));

	}

	void CodeGenerator::count_function_arguments(ASTreeNS::ASTNode_t* node, size_t& num_args){
		assert(node != nullptr);
		assert(node->key.code == Operator::COMMA);

		if (node->right() != nullptr){
			++num_args;
			local_offsets->emplace(node->right()->key.lexem, (num_args + 1) * 8);

			if (node->left() != nullptr) count_function_arguments(node->left(), num_args);
		}
	}

	void CodeGenerator::generate_return(ASTreeNS::ASTNode_t* node){
		generate_expression(node->right());

		instructions.push_back(new Assembly::SubReg2Val(Assembly::Registers::RSP, cur_local_vars_num * 8));
		instructions.push_back(new Assembly::MovReg2Reg(Assembly::Registers::RSP, Assembly::Registers::RBP));
		instructions.push_back(new Assembly::PopReg(Assembly::Registers::RBP));
		instructions.push_back(new Assembly::Ret());
	}

	void CodeGenerator::generate_branching(ASTreeNS::ASTNode_t* node){
		assert(node != nullptr);
		assert(node->key.code == Operator::IF);

		//instructions.push_back(new Assembly::PushReg(Assembly::Registers::R10));

		generate_expression(node->left()->left());
		instructions.push_back(new Assembly::MovReg2Reg(Assembly::Registers::R12, Assembly::Registers::R10));

		generate_expression(node->left()->right());
		instructions.push_back(new Assembly::MovReg2Reg(Assembly::Registers::R13, Assembly::Registers::R10));

		//instructions.push_back(new Assembly::PopReg(Assembly::Registers::R10));

		instructions.push_back(new Assembly::CmpReg2Reg(Assembly::Registers::R12, Assembly::Registers::R13));

		switch (node->left()->key.code){
			case Operator::EQL:
				instructions.push_back(new Assembly::Jz(num_blocks));
				instructions.push_back(new Assembly::Jmp(num_blocks + 1));
				break;

			case Operator::NEQL:
				instructions.push_back(new Assembly::Jnz(num_blocks));
				instructions.push_back(new Assembly::Jmp(num_blocks + 1));
				break;

			case Operator::EQLESS:
				instructions.push_back(new Assembly::Jle(num_blocks));
				instructions.push_back(new Assembly::Jmp(num_blocks + 1));
				break;

			case Operator::EQMORE:
				instructions.push_back(new Assembly::Jge(num_blocks));
				instructions.push_back(new Assembly::Jmp(num_blocks + 1));
				break;

			case Operator::LESS:
				instructions.push_back(new Assembly::Jl(num_blocks));
				instructions.push_back(new Assembly::Jmp(num_blocks + 1));
				break;

			case Operator::MORE:
				instructions.push_back(new Assembly::Jg(num_blocks));
				instructions.push_back(new Assembly::Jmp(num_blocks + 1));
				break;

			default:
				assert("Wrong branching format" && false);
		}

		generate_block(node->right()->right());
		generate_block(node->right()->left ());
	}

	void CodeGenerator::generate_exit(ASTreeNS::ASTNode_t* node){
		assert(node != nullptr);
		assert(node->key.code == Operator::EXIT);

		instructions.push_back(new Assembly::MovVal2Reg(Assembly::Registers::RAX, 60));
		instructions.push_back(new Assembly::MovVal2Reg(Assembly::Registers::RDI, 0));
		instructions.push_back(new Assembly::Syscall());
	}

	void CodeGenerator::generate_print(ASTreeNS::ASTNode_t* node){
		assert(node != nullptr);
		assert(node->key.code == Operator::WRITE);

		instructions.push_back(new Assembly::MovMem2Reg(Assembly::Registers::RAX, "num_format"));
		generate_expression(node->right());
		instructions.push_back(new Assembly::PushReg(Assembly::Registers::R10));
		instructions.push_back(new Assembly::Call("_vprintf"));
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
