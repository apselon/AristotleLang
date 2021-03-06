#pragma once
#include "CompLib.hpp"


namespace Assembly {
	const int UNUSED = 0;

		enum Spec_t {
			ORDINARY = 0,
			LABEL = 1,
			JUMP  = 2,
		};

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

		namespace JMP {
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

		virtual void set_offset(int32_t) {return  ;}
		virtual const char* assembly()   {return 0;}
		virtual const char* string()     {return 0;}
		virtual const uint8_t* elf()     {return 0;}
		virtual size_t size()            {return 0;}
		virtual Spec_t spec_type()       {return ORDINARY;}
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
				static uint8_t output[3] = {};
				output[0] = Binary::get_prefix(src, dst);
				output[1] = Binary::MOV::REG;
				output[2] = reg_mask(0b11000000, src, dst);

				return output;
			}
	};

	class MovVal2Reg: public Instruction {
	private:
		Registers::Reg dst;
		int32_t val;
	
	public:
		MovVal2Reg(Registers::Reg dst, int32_t val): dst(dst), val(val) {}

		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\t\tmov %s, %d", Registers::names[dst], val);
			return output;
		}

		size_t size(){
			return 7;
		}

		const uint8_t* elf(){
			static uint8_t output[7] = {};

			if (dst <= Registers::RDI){
				output[0] = Binary::REX::W; 
			}

			else {
				output[0] = Binary::REX::WB;
			}

			output[1] = Binary::MOV::NUM;
			output[2] = reg_mask(0b11000000, dst);
			
			const uint8_t* val_code = reinterpret_cast<const uint8_t*>(&val);
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

		const uint8_t* elf(){
			static uint8_t output[4] = {Binary::REX::WR, Binary::MOV::MEM, 0x55 };

			const uint8_t* val_code = reinterpret_cast<const uint8_t*>(&offset);
			memcpy(output + 3, val_code, 1);

			return output;
		}

	};
	
	class MovReg2Mem: public Instruction {
	private:
		Registers::Reg src_reg = Registers::NOT_REG;
		Registers::Reg dst;
		int32_t offset = 0;

	public:
		MovReg2Mem(Registers::Reg dst, int32_t offset, Registers::Reg src_reg): src_reg(src_reg), dst(dst), offset(offset) {}
		MovReg2Mem(int32_t offset, Registers::Reg src_reg): src_reg(src_reg), offset(offset) {}

		const char* assembly(){

			static char output[128] = "";

			if (src_reg != Registers::NOT_REG){
				sprintf(output, "\t\tmov [%s + %d], %s", Registers::names[dst], offset, Registers::names[src_reg]);
			}

			else {
				sprintf(output, "\t\tmov [%d], %s", offset, Registers::names[src_reg]);
			}

			return output;
		}

		size_t size(){
			return 7;
		}

		const uint8_t* elf(){
			static uint8_t output[7] = {0x90, Binary::MOV::REG, 0x95};
			output[0] = Binary::get_prefix(src_reg, dst);

			const uint8_t* val_code = reinterpret_cast<const uint8_t*>(&offset);
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

		const uint8_t* elf(){
			static uint8_t output[3] = {0x90, Binary::OP::ADD, 0x90};
			output[0] = Binary::get_prefix(src, dst);
			output[2] = reg_mask(0b11000000, src, dst);
			return output;
		}
	};

	class AddVal2Reg: public Instruction {
	private:
		Registers::Reg dst;
		int8_t src = 0;

	public:	
		AddVal2Reg(Registers::Reg dst, int8_t src): dst(dst), src(src) {};

		const char* assembly(){

			static char output[128] = "";
			sprintf(output, "\t\tadd %s, %d", Registers::names[dst], src); 

			return output;
		}

		size_t size(){
			return 4;
		}

		const uint8_t* elf(){
			static uint8_t output[4] = {Binary::REX::W, 0x83, 0xC4};

			const uint8_t* val_code = reinterpret_cast<const uint8_t*>(&src);
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

		const uint8_t* elf(){
			static uint8_t output[3] = {0x90, Binary::OP::SUB, 0x90};
			output[0] = Binary::get_prefix(src, dst);
			output[2] = reg_mask(0b11000000, src, dst);
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

		const uint8_t* elf(){
			static uint8_t output[4] = {Binary::REX::W, 0x83, 0xEC};

			const uint8_t* val_code = reinterpret_cast<const uint8_t*>(&val);
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

		const uint8_t* elf(){
			static uint8_t output[4] = {Binary::REX::WRB, 0x0F, Binary::OP::IMUL, 0x90};
			output[3] = reg_mask(0b11000000, dst, src);
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

		const uint8_t* elf(){
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
		int32_t offset = 0;
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

		void set_offset(int32_t new_offset){
			offset = new_offset;
		}

		const char* string(){
			char* output = new char[128]();
			if (num == UNUSED){
				sprintf(output, "%s", label); 
			}

			else {
				sprintf(output, ".Block%ld", num); 
			}

			return output;
		}

		size_t size(){
			return 5;
		}


		const uint8_t* elf(){
			static uint8_t output[5] = {Binary::JMP::JMP}; 
		
			const uint8_t* val_code = reinterpret_cast<const uint8_t*>(&offset);
			memcpy(output + 1, val_code, 4);

			return output;
		}

		Spec_t spec_type(){
			return JUMP;
		}
	};

	class Jz: public Instruction {
	private:
		int32_t offset = 0;
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

		const char* string(){
			char* output = new char[128]();
			if (num == UNUSED){
				sprintf(output, "%s", label); 
			}

			else {
				sprintf(output, ".Block%ld", num); 
			}

			return output;
		}


		void set_offset(int32_t new_offset){
			offset = new_offset;
		}

		size_t size(){
			return 6;
		}

		const uint8_t* elf(){
			static uint8_t output[6] = {0x0F, Binary::JMP::JE}; 
		
			const uint8_t* val_code = reinterpret_cast<const uint8_t*>(&offset);
			memcpy(output + 2, val_code, 4);

			return output;
		}

		Spec_t spec_type(){
			return JUMP;
		}
	};

	class Jnz: public Instruction {
	private:
		int32_t offset = 0;
		int64_t num = UNUSED;
		const char* label = nullptr;

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

		const char* string(){
			char* output = new char[128]();
			if (num == UNUSED){
				sprintf(output, "%s", label); 
			}

			else {
				sprintf(output, ".Block%ld", num); 
			}

			return output;
		}


		void set_offset(int32_t new_offset){
			offset = new_offset;
		}

		size_t size(){
			return 6;
		}

		const uint8_t* elf(){
			static uint8_t output[6] = {0x0F, Binary::JMP::JNE}; 
		
			const uint8_t* val_code = reinterpret_cast<const uint8_t*>(&offset);
			memcpy(output + 2, val_code, 4);

			return output;
			
		}

		Spec_t spec_type(){
			return JUMP;
		}
	};

	class Jg: public Instruction {
	private:
		int32_t offset = 0;
		int64_t num = -1;
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

		const char* string(){
			char* output = new char[128]();
			if (num == -1){
				sprintf(output, "%s", label); 
			}

			else {
				sprintf(output, ".Block%ld", num); 
			}

			return output;
		}


		void set_offset(int32_t new_offset){
			offset = new_offset;
		}

		size_t size(){
			return 6;
		}

		const uint8_t* elf(){
			static uint8_t output[6] = {0x0F, Binary::JMP::JG}; 
		
			const uint8_t* val_code = reinterpret_cast<const uint8_t*>(&offset);
			memcpy(output + 2, val_code, 4);

			return output;
		}

		Spec_t spec_type(){
			return JUMP;
		}
	};

	class Jge: public Instruction {
	private:
		int32_t offset = 0;
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

		const char* string(){
			char* output = new char[128]();
			if (num == UNUSED){
				sprintf(output, "%s", label); 
			}

			else {
				sprintf(output, ".Block%ld", num); 
			}

			return output;
		}


		void set_offset(int32_t new_offset){
			offset = new_offset;
		}

		size_t size(){
			return 6;
		}

		const uint8_t* elf(){
			static uint8_t output[6] = {0x0F, Binary::JMP::JGE}; 
		
			const uint8_t* val_code = reinterpret_cast<const uint8_t*>(&offset);
			memcpy(output + 2, val_code, 4);

			return output;
		}

		Spec_t spec_type(){
			return JUMP;
		}
	};


	class Jl: public Instruction {
	private:
		int32_t offset = 0;
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

		const char* string(){
			char* output = new char[128]();
			if (num == UNUSED){
				sprintf(output, "%s", label); 
			}

			else {
				sprintf(output, ".Block%ld", num); 
			}

			return output;
		}


		void set_offset(int32_t new_offset){
			offset = new_offset;
		}

		size_t size(){
			return 6;
		}

		const uint8_t* elf(){
			static uint8_t output[6] = {0x0F, Binary::JMP::JL}; 
		
			const uint8_t* val_code = reinterpret_cast<const uint8_t*>(&offset);
			memcpy(output + 2, val_code, 4);

			return output;
		}

		Spec_t spec_type(){
			return JUMP;
		}
	};

	class Jle: public Instruction {
	private:
		int32_t offset = 0;
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

		const char* string(){
			char* output = new char[128]();
			if (num == UNUSED){
				sprintf(output, "%s", label); 
			}

			else {
				sprintf(output, ".Block%ld", num); 
			}

			return output;
		}


		void set_offset(int32_t new_offset){
			offset = new_offset;
		}

		size_t size(){
			return 6;
		}

		const uint8_t* elf(){
			static uint8_t output[6] = {0x0F, Binary::JMP::JLE}; 
		
			const uint8_t* val_code = reinterpret_cast<const uint8_t*>(&offset);
			memcpy(output + 2, val_code, 4);

			return output;
		}

		Spec_t spec_type(){
			return JUMP;
		}
	};

	class Call: public Instruction {
	private:
		int32_t offset = 0;
		const char* label;

	public:	
		Call(const char* label): label(label) {};

		const char* assembly(){
			static char output[128] = "";
			sprintf(output, "\t\tcall %s", label); 

			return output;
		}

		void set_offset(int32_t new_offset){
			offset = new_offset;
		}

		const char* string(){
			char* output = new char[128]();
			sprintf(output, "%s", label); 
			return output;
		}


		size_t size(){
			return 5;
		}

		const uint8_t* elf(){
			static uint8_t output[5] = {Binary::JMP::CALL}; 
		
			const uint8_t* val_code = reinterpret_cast<const uint8_t*>(&offset);
			memcpy(output + 1, val_code, 4);

			return output;
		}

		Spec_t spec_type(){
			return JUMP;
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

		size_t size(){
			return 3;
		}

		const uint8_t* elf(){
			static uint8_t output[3] = {0x90, Binary::CMP::REG, 0x90}; 
			output[0] = Binary::get_prefix(src, dst);
			output[2] = reg_mask(0b11000000, src, dst);

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

		size_t size(){
			if (src <= Assembly::Registers::RDI){
				return 1;
			}

			return 2;
		}

		const uint8_t* elf(){
			static uint8_t output[2] = {}; 
			if (src <= Assembly::Registers::RDI){
				output[0] = reg_mask(Binary::PUSH::REG, src);
			}

			else {
				output[0] = Binary::REX::B;
				output[1] = reg_mask(Binary::PUSH::REG, src);
			}

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

		size_t size(){
				return 1;
		}

		const uint8_t* elf(){
			static uint8_t output[1] = {0x90}; 
			output[0] = reg_mask(Binary::POP::REG, dst);

			return output;
		}
	};

//===========================================================================//
//                               LABEL 
//===========================================================================//

	class Label: public Instruction {
	private:
		const char* name  = nullptr;
		const int64_t num = -1;
		int32_t offset = 0;
	
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

		const char* string(){
			char* output = new char[128]();
			if (num == -1){
				sprintf(output, "%s", name); 
			}
			
			else {
				sprintf(output, ".Block%ld", num); 
			}

			return output;
		}

		size_t size(){
			return 0;
		}

		void set_offset(int32_t new_offset){
			offset = new_offset;
		}

		Spec_t spec_type(){
			return LABEL;
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

		size_t size(){
			return 2;	
		}

		const uint8_t* elf(){
			static uint8_t output[2] = {0x0F, 0x05}; 
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
