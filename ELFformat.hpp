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
		const uint16_t PHNUM     = 0x0002;

		const uint16_t SHENTSIZE = 0x0040;
		const uint16_t SHNUM     = 0x003; //???

	}


	struct header {
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
}
