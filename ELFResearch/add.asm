section .text
		global _start

_start:
		mov rax, 0x3
		mov rbx, 0x5
		add rax, rbx
		
		mov rax, 60d
		mov rdi, 0
		syscall
