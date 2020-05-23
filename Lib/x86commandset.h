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

	class SubReg2Reg: public Instruction {
	private:
		Registers::Reg dst;
		Registers::Reg src;

		SubReg2Reg(Registers::Reg dst, Registers::Reg src): dst(dst), src(src) {};
	public:	
		const char* assembly(){

			static char output[128] = "";
			sprintf(output, "\tsub %s, %s", Registers::names[dst], Registers::names[src]);

			return output;
		}
	};

	class MulReg2Reg: public Instruction {
	private:
		Registers::Reg dst;
		Registers::Reg src;

		MulReg2Reg(Registers::Reg dst, Registers::Reg src): dst(dst), src(src) {};
	public:	
		const char* assembly(){

			static char output[128] = "";
			sprintf(output, "\tmul %s, %s", Registers::names[dst], Registers::names[src]);

			return output;
		}
	};

	class DivReg2Reg: public Instruction {
	private:
		Registers::Reg dst;
		Registers::Reg src;

		DivReg2Reg(Registers::Reg dst, Registers::Reg src): dst(dst), src(src) {};
	public:	
		const char* assembly(){

			static char output[128] = "";
			sprintf(output, "\tdiv %s, %s", Registers::names[dst], Registers::names[src]);

			return output;
		}
	};
}
