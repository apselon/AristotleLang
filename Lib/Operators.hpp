#pragma once

namespace Operator {

	const int NUM_OPS = 26;
	const int NUM_BRACKS = 4;

	enum type {
		MATH,
		LANG
	};

	enum code {
		BLOCK = -3,
		CONNECT = -2,
		NOT_OP = -1,
		ADD ,
		MUL ,
		SUB ,
		DIV ,
		SQRT ,
		NOT ,
		ASSGN ,
		COMMA ,
		DEC_FUNC ,
		DEC_VAR ,
		IF ,
		ELSE,
		WHILE ,
		O_BRACK ,
		C_BRACK ,
		CALL ,
		RETURN,
		EQL ,
		LESS ,
		MORE ,
		EQLESS ,
		EQMORE ,
		NEQL  ,
		EXIT ,
		WRITE ,
		READ,
	};

	const char* op_code_strs[] = {
		"ADD", //0
		"MUL", //1
		"SUB", //2
		"DIV", //3
		"SQRT", //4
		"NOT", //5
		"ASSGN", //6
		"COMMA", //7
		"DEC_FUNC", //8
		"DEC_VAR", //9
		"IF", //10
		"ELSE", //11
		"WHILE", //12
		"O_BRACK", //13
		"C_BRACK", //14
		"CALL", //15
		"RETURN", //16
		"JE", //17
		"JB", //18
		"JA", //19
		"JBE", //20
		"JAE", //21
		"JNE", //22
		"EXIT", //23
		"OUT", //24
		"IN",
	};

	const char* op_names[] = {
		"with", //0
		"times", //1
		"without", //2
		"split", //3
		"root", //4
		"not", //5
		"Let", //6
		",", //7
		"Theurgy", //8
		"Idea", //9
		"Criterion", //10
		"Else", //11
		"While", //12
		"{", //13
		"}", //14
		"Ritual", //15
		"Catharsis", //16
		"eq", //17
		"less", //18
		"more", //19
		"leseq", //20
		"moreq", //21
		"neql", //22
		"Thanks", //23
		"Write", //24
		"Read",
	};

	const char* o_brack_names[] = {
		"ideed,",
		"obviously,",
		"definetly,",
		"also,"
	};

	const char* c_brack_names[] = {
		"overall.",
		"hence.",
		"nevertheless.",
		"whereas.",
	};
}
