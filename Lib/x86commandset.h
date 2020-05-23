#include "CompLib.hpp"

namespace Assembly {

	namespace Registers {
		enum Reg {
			NOT_REG = -1,
			RAX,
			RBX,
			RCX,
			RDX,
			RBP,
			RSP,
			RSI,
			RDI
		};
	
		const char* names[] = {
			"rax",
			"rbx",
			"rcx",
			"rdx",
			"rbp",
			"rsp",
			"rsi",
			"rdi"
		};

		const char codes[] = {

		};
	
	};
	
	
	class Instruction {
	public:
		virtual const char* assembly();
		virtual const char* elf();
		virtual size_t size();
	};
//===========================================================================//
//                                   MOV
//===========================================================================//

	class MovReg2Reg: public Instruction {
		private:
			Registers::Reg src;
			Registers::Reg dst;

		public:
			MovReg2Reg(Registers::Reg dst, Registers::Reg src): src(src), dst(dst){}

			const char* assembly(){

				static char output[128] = "";
				sprintf(output, "\tmov %s, %s", Registers::names[dst], Registers::names[src]);

				return output;
			}
	};

	class MovVal2Reg: public Instruction {
	private:
		Registers::Reg dst;
		int64_t val;
	
	public:
		MovVal2Reg(Registers::Reg dst, int64_t val): dst(dst), val(val) {}

		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\tmov %s, %ld", Registers::names[dst], val);
			return output;
		}
	};

	class MovMem2Reg: public Instruction {
	private:
		Registers::Reg src_reg = Registers::NOT_REG;
		Registers::Reg dst;
		int64_t offset = 0;

	public:
		MovMem2Reg(Registers::Reg dst, Registers::Reg src_reg, int64_t offset): src_reg(src_reg), dst(dst), offset(offset) {}
		MovMem2Reg(Registers::Reg dst, int64_t offset): dst(dst), offset(offset) {}

		const char* assembly(){

			static char output[128] = "";

			if (src_reg != Registers::NOT_REG){
				sprintf(output, "\tmov %s, [%s + %ld]", Registers::names[dst], Registers::names[src_reg], offset);
			}

			else {
				sprintf(output, "\tmov %s, [%ld]", Registers::names[dst], offset);
			}

			return output;
		}

	};
	
	class MovReg2Mem: public Instruction {
	private:
		Registers::Reg src_reg = Registers::NOT_REG;
		Registers::Reg dst;
		int64_t offset = 0;

	public:
		MovReg2Mem(Registers::Reg dst, int64_t offset, Registers::Reg src_reg): src_reg(src_reg), dst(dst), offset(offset) {}
		MovReg2Mem(int64_t offset, Registers::Reg src_reg): src_reg(src_reg), offset(offset) {}

		const char* assembly(){

			static char output[128] = "";

			if (src_reg != Registers::NOT_REG){
				sprintf(output, "\tmov [%s + %ld], %s", Registers::names[dst], offset, Registers::names[src_reg]);
			}

			else {
				sprintf(output, "\tmov [%ld], %s", offset, Registers::names[src_reg]);
			}

			return output;
		}

	};

//===========================================================================//
//                                   ADD
//===========================================================================//

	class AddReg2Reg: public Instruction {
	private:
		Registers::Reg dst;
		Registers::Reg src;

		AddReg2Reg(Registers::Reg dst, Registers::Reg src): dst(dst), src(src) {};
	public:	
		const char* assembly(){

			static char output[128] = "";
			sprintf(output, "\tadd %s, %s", Registers::names[dst], Registers::names[src]);

			return output;
		}
	};

	class AddMem2Reg: public Instruction {
	private:
		Registers::Reg src_reg = Registers::NOT_REG;
		Registers::Reg dst;
		int64_t offset = 0;

	public:
		AddMem2Reg(Registers::Reg dst, Registers::Reg src_reg, int64_t offset): src_reg(src_reg), dst(dst), offset(offset) {}
		AddMem2Reg(Registers::Reg dst, int64_t offset): dst(dst), offset(offset) {}

		const char* assembly(){

			static char output[128] = "";

			if (src_reg != Registers::NOT_REG){
				sprintf(output, "\tadd %s, [%s + %ld]", Registers::names[dst], Registers::names[src_reg], offset);
			}

			else {
				sprintf(output, "\tadd %s, [%ld]", Registers::names[dst], offset);
			}

			return output;
		}

	};

//===========================================================================//
//                                    SUB
//===========================================================================//

	class SubReg2Reg: public Instruction {
	private:
		Registers::Reg dst;
		Registers::Reg src;

	public:	
		SubReg2Reg(Registers::Reg dst, Registers::Reg src): dst(dst), src(src) {};

		const char* assembly(){

			static char output[128] = "";
			sprintf(output, "\tsub %s, %s", Registers::names[dst], Registers::names[src]);

			return output;
		}
	};

//===========================================================================//
//                                   MUL
//===========================================================================//

	class MulReg2Reg: public Instruction {
	private:
		Registers::Reg dst;
		Registers::Reg src;

	public:	
		MulReg2Reg(Registers::Reg dst, Registers::Reg src): dst(dst), src(src) {};

		const char* assembly(){

			static char output[128] = "";
			sprintf(output, "\tmul %s, %s", Registers::names[dst], Registers::names[src]);

			return output;
		}
	};

//===========================================================================//
//                                  DIV
//===========================================================================//

	class DivReg2Reg: public Instruction {
	private:
		Registers::Reg dst;
		Registers::Reg src;

	public:	
		DivReg2Reg(Registers::Reg dst, Registers::Reg src): dst(dst), src(src) {};

		const char* assembly(){

			static char output[128] = "";
			sprintf(output, "\tdiv %s, %s", Registers::names[dst], Registers::names[src]);

			return output;
		}
	};

//===========================================================================//
//                                  RET
//===========================================================================//

	class Ret: public Instruction {
	public:	
		const char* assembly(){
			static char output[128] = "\t ret";
			return output;
		}
	};

//===========================================================================//
//                                  JMP
//===========================================================================//
//
	class Jmp: public Instruction {
	private:
		int64_t offset = 0;
		const char* label;

	public:	
		Jmp(const char* label): label(label) {};

		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\tjmp %s", label); 

			return output;
		}
	};

	class Jz: public Instruction {
	private:
		int64_t offset = 0;
		const char* label;

	public:	
		Jz(const char* label): label(label) {};

		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\tjz %s", label); 

			return output;
		}
	};

	class Jnz: public Instruction {
	private:
		int64_t offset = 0;
		const char* label;

	public:	
		Jnz(const char* label): label(label) {};

		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\tjnz %s", label); 

			return output;
		}
	};

	class Jg: public Instruction {
	private:
		int64_t offset = 0;
		const char* label;

	public:	
		Jg(const char* label): label(label) {};

		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\tjg %s", label); 

			return output;
		}
	};

	class Jge: public Instruction {
	private:
		int64_t offset = 0;
		const char* label;

	public:	
		Jge(const char* label): label(label) {};

		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\tjge %s", label); 

			return output;
		}
	};

	class Jl: public Instruction {
	private:
		int64_t offset = 0;
		const char* label;

	public:	
		Jl(const char* label): label(label) {};

		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\tjl %s", label); 

			return output;
		}
	};

	class Jle: public Instruction {
	private:
		int64_t offset = 0;
		const char* label;

	public:	
		Jle(const char* label): label(label) {};

		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\tjle %s", label); 

			return output;
		}
	};

	class CmpReg2Reg: public Instruction {
	private:
		Registers::Reg dst;
		Registers::Reg src;
	
	public:
		CmpReg2Reg(Registers::Reg dst, Registers::Reg src): dst(dst), src(src){};
		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\tcmp %s, %s", Registers::names[dst], Registers::names[src]); 
			return output;
		}
	};

}
