#pragma once
#include "../Lib/CompLib.hpp"
#include "../Lib/Operators.hpp"

namespace TokenizerNS {
	enum token_type {
		NIL = -1,
		NUM,
		ID,
		OP,
		SPEC
	};

	struct Token {
		const char* lexem = nullptr;
		token_type type = NIL;
		Operator::code code = Operator::NOT_OP;

		Token() = default;
		Token(const char*);
		Token(const char*, token_type, Operator::code);
	};

	class Tokenizer {
	private:
		const char* program = nullptr;
		const char** lexems = nullptr;
		Token* tokens_      = nullptr;

		size_t num_tokens_  = 0;

	public:
		explicit Tokenizer(FILE* input_file);
		~Tokenizer();
		Token* tokens();
	};
};
