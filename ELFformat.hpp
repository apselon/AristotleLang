#include <cstdint>

namespace ELF {

	const uint32_t MAG_NUM  = 0x7F454C46; 

	namespace IntelExec86_64 {

		const uint8_t  CLASS     = 0x02;
		const uint8_t  DATA2LSB  = 0x01;
		const uint8_t  EV_CURENT = 0x01;
		const uint8_t  SYSV_ABI  = 0x00;
	
		const uint16_t ET_EXEC   = 0x0002;
		const uint16_t EM        = 0x003E;
	
		const uint16_t EHSIZE    = 0x0040;
		const uint16_t PHENTSIZE = 0x0038;
		const uint16_t SHENTSIZE = 0x0040;

	}

 // ============================================================================
 // ELF Header
 // ============================================================================

	struct Header {
		uint32_t EI_MAG;      //0x7F454C46
	
		uint8_t  EI_CLASS;    //64 BIT;
		uint8_t  EI_DATA;     //DATA2LSB (Little - Endian)
		uint8_t  EI_VERSION;
		uint8_t  EI_OSABI;    //Unix system V ABI
		uint8_t  EI_OSABIVER; 
	
		uint16_t E_TYPE;      //ET_EXEC - executable
		uint16_t E_MACHINE;   //EM86_64
		uint32_t E_VERSION; 	

		uint64_t E_ENTRY;     //entry point virtual address.
		uint64_t E_PHOFF;     //offset of the header table
		uint64_t E_SHOFF;     //offset of the section table
		uint32_t E_FLAGS;     //Intel i386 defines no flags
	
		uint16_t E_EHSIZE;    //size of elf header
		uint16_t E_PHENTSIZE; //size of each program header
		uint16_t E_PHNUM;     //number of program headers
		uint16_t E_SHENTSIZE; //section header size
		uint16_t E_SHNUM;     //number of section headers
		uint16_t E_SHSTRNDX;  //section header string index -- index of the .shstrtab section.
	};

 // ============================================================================
 // Program Header Table 
 // Contains information of how the executable should be put into the process virtual memory.
 // ============================================================================
 
	namespace SegType {
		uint32_t PT_NULL = 0x0000;
		uint32_t PT_LOAD = 0x0001;
		uint32_t PT_DYNAMIC = 0x0002;
	};

	namespace ProgFlags {
		uint32_t PF_X = 0x1; //use;
		uint32_t PF_W = 0x2; //write;
		uint32_t PF_R = 0x4; //read;
	}
 
	struct ProgramHeader {
		uint32_t P_TYPE;
		uint32_t P_FLAGS;
		uint64_t P_OFFSET; //Segment offset;
		uint64_t P_VADDR;  //virtual memory offset to load this segment to
		uint64_t P_PADDR;  //usually = vaddr
		uint64_t P_FILESZ; //segment size in file
		uint64_t P_MEMSZ;  //segment size in mem
		uint64_t P_ALLIGN; //usually 0x1000

	};
}
