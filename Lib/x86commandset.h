#pragma once
#include "CompLib.hpp"


namespace Assembly {
	const int UNUSED = 0;

	namespace Registers {
		enum Reg {
			NOT_REG = -1,
			RAX,
			RBX,
			RCX,
			RDX,
			RSP,
			RBP,
			RSI,
			RDI,
			R10,
			R11,
			R12,
			R13,
			R14,
			R15
		};

		const uint8_t codes[] {
			0, //rax
			3, //rbx
			1, //rcx
			2, //rdx
			4, //rsp
			5, //rbp
			6, //rsi
			7, //rdi
			2, //r10
			3, //r11
			4, //r12
			5, //r13
			6, //r14

		};
	
		const char* names[] = {
			"rax",
			"rbx",
			"rcx",
			"rdx",
			"rsp",
			"rbp",
			"rsi",
			"rdi",
			"r10",
			"r11",
			"r12",
			"r13",
			"r14",
			"r15"
		};
	};
	
	namespace Binary {
	
		namespace REX {
			constexpr uint8_t O   = 0x40;
			constexpr uint8_t B   = 0x41;
			constexpr uint8_t W   = 0x48;
			constexpr uint8_t WB  = 0x49;
			constexpr uint8_t WR  = 0x4C;
			constexpr uint8_t WRB = 0x4D;
		}
		
		namespace PUSH {
			enum {
				REG = 0x50,
				NUM = 0x68,
			};
		}
	
		namespace POP {
			enum {
				REG = 0x58,
			};
		}
	
		namespace MOV {
			enum {
				NUM = 0xC7,
				REG = 0x89,
				MEM = 0x8B
			};
		};
	
		namespace CMP {
			enum {
				REG = 0x39
			};
		}
	
		namespace OP {
			enum {
				ADD  = 0x01,
				SUB  = 0x29,
				IMUL = 0xAF,
			};
		}

		uint8_t get_prefix(Assembly::Registers::Reg src, Assembly::Registers::Reg dst){
			assert(src != Assembly::Registers::NOT_REG && dst != Assembly::Registers::NOT_REG);

			if (src > Assembly::Registers::RDI && dst > Assembly::Registers::RDI){
				return Binary::REX::WRB;
			}

			if (src > Assembly::Registers::RDI && dst <= Assembly::Registers::RDI){
				return Binary::REX::WR;
			}

			else if (src <= Assembly::Registers::RDI && dst > Assembly::Registers::RDI){
				return Binary::REX::WB;
			}
			
			else {
				return Binary::REX::W;
			}
		}
	};

	uint8_t reg_mask(uint8_t mask, Assembly::Registers::Reg src, Assembly::Registers::Reg dst){
		return (mask | (Registers::codes[src] << 3) | Registers::codes[dst]);
	}

	uint8_t reg_mask(uint8_t mask, Assembly::Registers::Reg dst){
		return (mask | Registers::codes[dst]);
	}
	
	
	class Instruction {
	public:
		virtual const char* assembly() {return 0;};
		virtual const uint8_t* elf() { return 0;}
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

			size_t size(){
				return 3;
			}

			const uint8_t* elf(){
				static uint8_t output[3] = {Binary::get_prefix(src, dst), Binary::MOV::REG, reg_mask(0b11000000, src, dst)};
				return output;
			}
	};

	class MovVal2Reg: public Instruction {
	private:
		Registers::Reg dst;
		int64_t val;
	
	public:
		MovVal2Reg(Registers::Reg dst, int32_t val): dst(dst), val(val) {}

		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\t\tmov %s, %ld", Registers::names[dst], val);
			return output;
		}

		size_t size(){
			return 7;
		}

		const uint8_t* elf(){
			static uint8_t output[7] = {Binary::REX::W, Binary::MOV::NUM, reg_mask(0b11000000, dst)};
			uint8_t* val_code = reinterpret_cast<uint8_t*>(&val);
			memcpy(output + 3, val_code, 4);
			return output;
		}
	};

	class MovMem2Reg: public Instruction {
	private:
		Registers::Reg src_reg = Registers::NOT_REG;
		const char* src_label = nullptr;
		Registers::Reg dst;
		int8_t offset = UNUSED;

	public:
		MovMem2Reg(Registers::Reg dst, Registers::Reg src_reg, int8_t offset): src_reg(src_reg), dst(dst), offset(offset) {}

		MovMem2Reg(Registers::Reg dst, const char* src_label): src_label(src_label), dst(dst) {}
		MovMem2Reg(Registers::Reg dst, int8_t offset): dst(dst), offset(offset) {}

		const char* assembly(){

			static char output[128] = "";

			if (src_reg != Registers::NOT_REG){
				sprintf(output, "\t\tmov %s, [%s + %d]", Registers::names[dst], Registers::names[src_reg], offset);
			}

			else if (offset != UNUSED) {
				sprintf(output, "\t\tmov %s, [%d]", Registers::names[dst], offset);
			}

			else {
				sprintf(output, "\t\tmov %s, %s", Registers::names[dst], src_label);
			}

			return output;
		}

		size_t size(){
			return 4;
		}

		uint8_t* elf(){
			static uint8_t output[4] = {Binary::REX::W, Binary::MOV::MEM, 0x55 };

			uint8_t* val_code = reinterpret_cast<uint8_t*>(&offset);
			memcpy(output + 3, val_code, 1);

			return output;
		}

	};
	
	class MovReg2Mem: public Instruction {
	private:
		Registers::Reg src_reg = Registers::NOT_REG;
		Registers::Reg dst;
		int64_t offset = 0;

	public:
		MovReg2Mem(Registers::Reg dst, int32_t offset, Registers::Reg src_reg): src_reg(src_reg), dst(dst), offset(offset) {}
		MovReg2Mem(int32_t offset, Registers::Reg src_reg): src_reg(src_reg), offset(offset) {}

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

		size_t size(){
			return 7;
		}

		const uint8_t* elf(){
			static uint8_t output[7] = {Binary::REX::W, Binary::MOV::MEM, 0x95};

			uint8_t* val_code = reinterpret_cast<uint8_t*>(&offset);
			memcpy(output + 3, val_code, 4);

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

		size_t size(){
			return 3;
		}

		uint8_t* elf(){
			static uint8_t output[3] = {Binary::get_prefix(src, dst), Binary::OP::ADD, reg_mask(0b11000000, src, dst)};
			return output;
		}
	};

	/*
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
	*/

	class AddVal2Reg: public Instruction {
	private:
		Registers::Reg dst;
		int8_t src = 0;

	public:	
		AddVal2Reg(Registers::Reg dst, int64_t src): dst(dst), src(src) {};

		const char* assembly(){

			static char output[128] = "";
			sprintf(output, "\t\tadd %s, %d", Registers::names[dst], src); 

			return output;
		}

		size_t size(){
			return 4;
		}

		uint8_t* elf(){
			static uint8_t output[4] = {Binary::REX::W, 0x83, 0xC4};

			uint8_t* val_code = reinterpret_cast<uint8_t*>(&src);
			memcpy(output + 3, val_code, 1);
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

		size_t size(){
			return 3;
		}

		uint8_t* elf(){
			static uint8_t output[3] = {Binary::get_prefix(src, dst), Binary::OP::SUB, reg_mask(0b11000000, src, dst)};
			return output;
		}
	};


	class SubReg2Val: public Instruction {
	private:
		Registers::Reg dst;
		int8_t val;

	public:	
		SubReg2Val(Registers::Reg dst, int8_t val): dst(dst), val(val) {};

		const char* assembly(){

			static char output[128] = "";
			sprintf(output, "\t\tsub %s, %d", Registers::names[dst], val);

			return output;
		}

		size_t size(){
			return 4;
		}

		uint8_t* elf(){
			static uint8_t output[4] = {Binary::REX::W, 0x83, 0xEC};

			uint8_t* val_code = reinterpret_cast<uint8_t*>(&val);
			memcpy(output + 3, val_code, 1);
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
			sprintf(output, "\t\timul %s, %s", Registers::names[dst], Registers::names[src]);

			return output;
		}

		size_t size(){
			return 4;
		}

		uint8_t* elf(){
			static uint8_t output[4] = {Binary::REX::W, 0x0F, Binary::OP::IMUL, reg_mask(0b11000000, src, dst)};
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

		size_t size(){
			return 1;
		}

		uint8_t* elf(){
			static uint8_t output[1] = {0xC3};
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
		int64_t num = UNUSED;
		const char* label;

	public:	
		Jmp(const char* label): label(label) {};
		Jmp(int64_t num): num(num) {};

		const char* assembly(){
			static char output[128] = "";
			if (num == UNUSED){
				sprintf(output, "\t\tjmp %s", label); 
			}

			else {
				sprintf(output, "\t\tjmp .Block%ld", num); 
			}

			return output;
		}
	};

	class Jz: public Instruction {
	private:
		int64_t offset = 0;
		int64_t num = UNUSED;
		const char* label;

	public:	
		Jz(const char* label): label(label) {};
		Jz(int64_t num): num(num) {};

		const char* assembly(){
			static char output[128] = "";
			if (num == UNUSED){
				sprintf(output, "\t\tjz %s", label); 
			}
			
			else {
				sprintf(output, "\t\tjz .Block%ld", num); 
			}

			return output;
		}
	};

	class Jnz: public Instruction {
	private:
		int64_t offset = 0;
		int64_t num = UNUSED;
		const char* label;

	public:	
		Jnz(const char* label): label(label) {};
		Jnz(int64_t num): num(num) {};

		const char* assembly(){
			static char output[128] = "";
			if (num == UNUSED){
				sprintf(output, "\t\tjnz %s", label); 
			}

			else {
				sprintf(output, "\t\tjnz .Block%ld", num); 
			}

			return output;
		}
	};

	class Jg: public Instruction {
	private:
		int64_t offset = 0;
		int64_t num = UNUSED;
		const char* label = nullptr;

	public:	
		Jg(const char* label): label(label) {};
		Jg(int64_t num): num(num) {};

		const char* assembly(){
			static char output[128] = "";
			if (num == UNUSED){
				sprintf(output, "\t\tjg %s", label); 
			}

			else {
				sprintf(output, "\t\tjg .Block%ld", num); 
			}

			return output;
		}
	};

	class Jge: public Instruction {
	private:
		int64_t offset = 0;
		int64_t num = UNUSED;
		const char* label;

	public:	
		Jge(const char* label): label(label) {};
		Jge(int64_t num): num(num) {};

		const char* assembly(){
			static char output[128] = "";
			if (num == UNUSED){
				sprintf(output, "\t\tjge %s", label); 
			}

			else {
				sprintf(output, "\t\tjge .Block%ld", num); 
			}

			return output;
		}
	};


	class Jl: public Instruction {
	private:
		int64_t offset = 0;
		int64_t num = UNUSED;
		const char* label;

	public:	
		Jl(const char* label): label(label) {};
		Jl(int64_t num): num(num) {};

		const char* assembly(){
			static char output[128] = "";
			if (num == UNUSED){
				sprintf(output, "\t\tjl %s", label); 
			}

			else {
				sprintf(output, "\t\tjl .Block%ld", num); 
			}

			return output;
		}
	};

	class Jle: public Instruction {
	private:
		int64_t offset = 0;
		int64_t num = UNUSED;
		const char* label;

	public:	
		Jle(const char* label): label(label) {};
		Jle(int64_t num): num(num) {};

		const char* assembly(){
			static char output[128] = "";
			if (num == UNUSED){
				sprintf(output, "\t\tjle %s", label); 
			}

			else {
				sprintf(output, "\t\tjle .Block%ld", num); 
			}

			return output;
		}
	};

	class Call: public Instruction {
	private:
		int64_t offset = 0;
		const char* label;

	public:	
		Call(const char* label): label(label) {};

		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\t\tcall %s", label); 

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
				sprintf(output, ".Block%ld:", num); 
			}

			return output;
		}
	};

	class Array: public Instruction {
	private:
		const char* name  = nullptr;
		const char* val   = nullptr;
	
	public:
		Array(const char* name, const char* val): name(name), val(val) {}
		

		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "%s db %s", name, val); 
			return output;
		}
	};

	class Comment: public Instruction {
	private:
		const char* text = nullptr; 
		const int64_t num = UNUSED;
	
	public:
		explicit Comment(const char* text): text(text) {};
		explicit Comment(int num): num(num) {};

		const char* assembly(){

			static char output[128] = "";
			if (num == UNUSED){
				sprintf(output, ";%s", text); 
			}

			else {
				sprintf(output, ";.EndBlock%ld", num); 
			}

			return output;
		}
	};

	class Syscall: public Instruction {
	public:	
		Syscall(){};
		const char* assembly(){
			static char output[128] = "\t\tsyscall";
			return output;
		}
	};

	class Section: public Instruction {
	private:
		const char* name;
	
	public:
		Section(const char* name): name(name) {};
		
		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "section %s", name);
			
			return output;
		}
	};

	class Global: public Instruction {
	private:
		const char* name;
	
	public:
		Global(const char* name): name(name) {};
		
		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\t\tglobal %s", name);
			
			return output;
		}

	};

	class Extern: public Instruction {
	private:
		const char* name;
	
	public:
		Extern(const char* name): name(name) {};
		
		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\t\textern %s", name);
			
			return output;
		}

	};
}

/*
namespace Binary {

	namespace Registers {
		enum Reg32 {
			RAX = 0,
			RCX = 1,
			RDX = 2,
			RBX = 3,
			RBP = 4,
			RSP = 5,
			RSI = 6,
			RDI = 7,
		};

		enum Reg64 {
			R10 = 1,
			R11 = 2,
			R12 = 3,
			R13 = 4,
			R14 = 5,
			R15 = 6
		};
	};


	namespace Jump {
		namespace Near {
			enum {
				CALL = 0xE8,
				JMP  = 0xE9,
				RET  = 0xC3,
				JA   = 0x87,
				JAE  = 0x83,
				JB   = 0x82,
				JBE  = 0x86,
				JE   = 0x84,
				JNE  = 0x85,
				
				JG   = 0x8F,
				JGE  = 0x8D,
				JL   = 0x8C,
				JLE  = 0x8E,
			};
		}
	}

	uint8_t* AddR10R11(uint8_t* array){

		uint8_t code[] = {Prefix::REX::WRB, Op::ADD, 0xDA};

		memcpy(array, code, sizeof(code));	
		return array + sizeof(code);
	}

	uint8_t* SubR10R11(uint8_t* array){

		uint8_t code[] = {Prefix::REX::WRB, Op::SUB, 0xDA};

		memcpy(array, code, sizeof(code));	
		return array + sizeof(code);
	}

	uint8_t* ImulR10R11(uint8_t* array){

		uint8_t code[] = {0x48, 0x0F, 0xAF, 0xD3};

		memcpy(array, code, sizeof(code));	
		return array + sizeof(code);
	}

	uint8_t* PushR10(uint8_t* array){

		uint8_t code[] = {0x41, 0x52};

		memcpy(array, code, sizeof(code));	
		return array + sizeof(code);
	}

	uint8_t* PushRBP(uint8_t* array){

		uint8_t code[] = {0x55};

		memcpy(array, code, sizeof(code));	
		return array + sizeof(code);
	}

	uint8_t* PopRBP(uint8_t* array){

		uint8_t code[] = {0x5d};

		memcpy(array, code, sizeof(code));	
		return array + sizeof(code);
	}

	uint8_t* MovRSP2RBP(uint8_t* array){

		uint8_t code[] = {0x48, 0x89, 0xE5};

		memcpy(array, code, sizeof(code));	
		return array + sizeof(code);
	}

	uint8_t* MovRBPtoRSP(uint8_t* array){

		uint8_t code[] = {0x48, 0x89, 0xEC};

		memcpy(array, code, sizeof(code));	
		return array + sizeof(code);
	}

	uint8_t* MovNum2R10(uint32_t num, uint8_t* array){
		uint8_t code[] = {0x48, 0xC7, 0xC2};
		uint8_t* num_code = reinterpret_cast<uint8_t*>(&num);

		memcpy(array, code, sizeof(code));	
		array += sizeof(code);
		memcpy(array, num_code, 4);	
		return array + 4;
	}

	uint8_t* MovR102Mem(uint32_t offset, uint8_t* array){
		uint8_t code[] = {0x4C, 0x89, 0x95};
		uint8_t* offset_code = reinterpret_cast<uint8_t*>(&offset);

		memcpy(array, code, sizeof(code));	
		array += sizeof(code);
		memcpy(array, offset_code, 4);	
		return array + 4;
	}

	uint8_t* MovMem2R10(uint32_t offset, uint8_t* array){
		uint8_t code[] = {0x48, 0x8B, 0x95};
		uint8_t* offset_code = reinterpret_cast<uint8_t*>(&offset);

		memcpy(array, code, sizeof(code));	
		array += sizeof(code);
		memcpy(array, offset_code, 4);	
		return array + 4;
	}

	uint8_t* AddRSPNum(uint32_t num, uint8_t* array){
		uint8_t code[] = {0x48, 0x81, 0xC4};
		uint8_t* num_code = reinterpret_cast<uint8_t*>(&num);

		memcpy(array, code, sizeof(code));	
		array += sizeof(code);
		memcpy(array, num_code, 4);	
		return array + 4;
	}
	
	uint8_t* SubRSPNum(uint32_t num, uint8_t* array){
		uint8_t code[] = {0x48, 0x81, 0xEC};
		uint8_t* num_code = reinterpret_cast<uint8_t*>(&num);

		memcpy(array, code, sizeof(code));	
		array += sizeof(code);
		memcpy(array, num_code, 4);	
		return array + 4;
	}

	uint8_t* MovR10toR11(uint8_t* array){
		uint8_t code[] = {0x4D, 0x89, 0xD3};

		memcpy(array, code, sizeof(code));	
		array += sizeof(code);
		return array;
	}

	uint8_t* Ret(uint8_t* array){
		uint8_t code[] = {0xC3};

		memcpy(array, code, sizeof(code));	
		array += sizeof(code);
		return array;
	}
};
*/
