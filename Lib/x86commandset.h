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
			RDI,
			R10,
			R11,
			R12
		};
	
		const char* names[] = {
			"rax",
			"rbx",
			"rcx",
			"rdx",
			"rbp",
			"rsp",
			"rsi",
			"rdi",
			"r10",
			"r11",
			"r12"
		};

		const char codes[] = {

		};
	
	};
	
	
	class Instruction {
	public:
		virtual const char* assembly() {return 0;};
		virtual const char* elf() { return 0;}
		virtual size_t size() {return 0;};
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
				sprintf(output, "\t\tmov %s, %s", Registers::names[dst], Registers::names[src]);

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
			sprintf(output, "\t\tmov %s, %ld", Registers::names[dst], val);
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
				sprintf(output, "\t\tmov %s, [%s + %ld]", Registers::names[dst], Registers::names[src_reg], offset);
			}

			else {
				sprintf(output, "\t\tmov %s, [%ld]", Registers::names[dst], offset);
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
				sprintf(output, "\t\tmov [%s + %ld], %s", Registers::names[dst], offset, Registers::names[src_reg]);
			}

			else {
				sprintf(output, "\t\tmov [%ld], %s", offset, Registers::names[src_reg]);
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

	public:	
		AddReg2Reg(Registers::Reg dst, Registers::Reg src): dst(dst), src(src) {};

		const char* assembly(){

			static char output[128] = "";
			sprintf(output, "\t\tadd %s, %s", Registers::names[dst], Registers::names[src]);

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
				sprintf(output, "\t\tadd %s, [%s + %ld]", Registers::names[dst], Registers::names[src_reg], offset);
			}

			else {
				sprintf(output, "\t\tadd %s, [%ld]", Registers::names[dst], offset);
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
			sprintf(output, "\t\tsub %s, %s", Registers::names[dst], Registers::names[src]);

			return output;
		}
	};

	class SubReg2Val: public Instruction {
	private:
		Registers::Reg dst;
		int64_t val;

	public:	
		SubReg2Val(Registers::Reg dst, int64_t val): dst(dst), val(val) {};

		const char* assembly(){

			static char output[128] = "";
			sprintf(output, "\t\tsub %s, %ld", Registers::names[dst], val);

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
			sprintf(output, "\t\tmul %s, %s", Registers::names[dst], Registers::names[src]);

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
			sprintf(output, "\t\tdiv %s, %s", Registers::names[dst], Registers::names[src]);

			return output;
		}
	};

//===========================================================================//
//                                  RET
//===========================================================================//

	class Ret: public Instruction {
	public:	
		const char* assembly(){
			static char output[128] = "\t\tret";
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
			sprintf(output, "\t\tjmp %s", label); 

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
			sprintf(output, "\t\tjz %s", label); 

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
			sprintf(output, "\t\tjnz %s", label); 

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
			sprintf(output, "\t\tjg %s", label); 

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
			sprintf(output, "\t\tjge %s", label); 

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
			sprintf(output, "\t\tjl %s", label); 

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
			sprintf(output, "\t\tjle %s", label); 

			return output;
		}
	};

//===========================================================================//
//                                  CMP
//===========================================================================//

	class CmpReg2Reg: public Instruction {
	private:
		Registers::Reg dst;
		Registers::Reg src;
	
	public:
		CmpReg2Reg(Registers::Reg dst, Registers::Reg src): dst(dst), src(src){};
		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\t\tcmp %s, %s", Registers::names[dst], Registers::names[src]); 
			return output;
		}
	};

//===========================================================================//
//                                 PUSH
//===========================================================================//

	class PushReg: public Instruction {
	private:
		Registers::Reg src;
	
	public:
		PushReg(Registers::Reg src): src(src){};

		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\t\tpush %s", Registers::names[src]); 
			return output;
		}
	};

//===========================================================================//
//                                POP 
//===========================================================================//

	class PopReg: public Instruction {
	private:
		Registers::Reg dst;
	
	public:
		PopReg(Registers::Reg dst): dst(dst){};

		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\t\tpop %s", Registers::names[dst]); 
			return output;
		}
	};

//===========================================================================//
//                                POP 
//===========================================================================//

	class Label: public Instruction {
	private:
		const char* name  = nullptr;
		const int64_t num = -1;
	
	public:
		explicit Label(int64_t num): num(num) {} 
		explicit Label(const char* name): name(name) {}
		

		const char* assembly(){
			static char output[128] = "";
			if (num == -1){
				sprintf(output, "%s:", name); 
			}
			
			else {
				sprintf(output, ".Block%ld", num); 
			}

			return output;
		}
	};

	class Comment: public Instruction {
	private:
		const char* text = nullptr; 
		const int64_t num = -666;
	
	public:
		explicit Comment(const char* text): text(text) {};
		explicit Comment(int num): num(num) {};

		const char* assembly(){

			static char output[128] = "";
			if (num == -666){
				sprintf(output, ";%s", text); 
			}

			else {
				sprintf(output, ";.EndBlock%ld", num); 
			}

			return output;
		}
	};
}
