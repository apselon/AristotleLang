#include "Tokenizer.hpp"

namespace TokenizerNS {
	void skip_spaces(const char** text){
		while (isspace(**text)){
			++(*text);
		}
	}

	Operator::code get_opcode(const char* lexem){
	
		for (int i = 0; i < Operator::NUM_OPS; ++i){
			if (strcmp(lexem, Operator::op_names[i]) == 0){
				return (Operator::code)(i);
			}
		}
	
		for (int i = 0; i < Operator::NUM_BRACKS; ++i){
			if (strcmp(lexem, Operator::o_brack_names[i]) == 0){
				return Operator::O_BRACK;
			}
	
			if (strcmp(lexem, Operator::c_brack_names[i]) == 0){
				return Operator::C_BRACK;
			}
		}
	
		return Operator::NOT_OP;
	}

	Token::Token(const char* lexem): lexem(lexem) {

		code = get_opcode(lexem);

		if (code != Operator::NOT_OP){
			type = TokenizerNS::OP;
		}

		else if (isdigit(*lexem)){
			type = TokenizerNS::NUM;
		}

		else {
			type = TokenizerNS::ID;
		}
	}

	Token::Token(const char* lexem, TokenizerNS::token_type type, Operator::code code): lexem(lexem), type(type), code(code){};

	Tokenizer::Tokenizer(FILE* input_file){

		fseek(input_file, 0L, SEEK_END);
		size_t file_size = ftell(input_file);

		rewind(input_file);
		
		program = new const char [file_size + 128 ]();
		lexems  = new const char*[Consts::BUF_SIZE]();

		int offset = 0;

		while (*program){
			skip_spaces(&program);
			if (!(*program)) break;

			sscanf(program, "%ms%n", lexems++, &offset);
			program += offset;
			++num_tokens_;
		}

		lexems -= num_tokens_;

		tokens_ = new Token[num_tokens_]();
	
		for (size_t i = 0; i < num_tokens_; ++i){
			tokens_[i] = Token(lexems[i]);
		}
	}
};
