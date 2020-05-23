#pragma once

#define SPEC_CONNECTION TokenizerNS::Token("CONNECTION", TokenizerNS::SPEC, Operator::CONNECT)
#define SPEC_BLOCK TokenizerNS::Token("BLOCK", TokenizerNS::SPEC, Operator::BLOCK)
#define SPEC_OP TokenizerNS::Token("OP", TokenizerNS::SPEC, Operator::BLOCK)

#define BASIC_INSTRUCTION_X86(mnem)                                     \
	void mnem##_(FILE* output, const char* dst, const char* src){ \
		fprintf(output, "\t\t%s %s, %s\n", #mnem, dst, src);      \
	}                                                             \
  
