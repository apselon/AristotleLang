#include "codegenerator.hpp"

namespace CodeGeneratorNS {
	CodeGenerator::CodeGenerator(const ASTreeNS::ASTree& tree){
		cur	= tree.root();
		
	}

	void CodeGenerator::generate_operator(ASTreeNS::ASTNode_t* node){
		assert(node != nullptr);

		switch (cur->key.code){
		case Operator::IF:
			generate_branching(node);
			break;
		case Operator::DEC_VAR:
			generate_var_declaration(node);
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
			generate_input(node);
			break;
		case Operator::SQRT:
			generate_sqrt(node);
			break;
		default:
			generate_nop();
			printf("Wrong operator Node\n");
			break;
		}
	}

	void CodeGenerator::generate_body(ASTreeNS::ASTNode_t* node){
		while (node->right() != nullptr){
			generate_operator(node->right());

			node = node->left();
		}
	}

	void generate_var_declaration(ASTreeNS::ASTNode_t* node){
		
	}
}
