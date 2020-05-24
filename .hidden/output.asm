section .text
num_format db '%d', 10d, 0
		extern _vprintf
		global _start
.Block0:
_start:
;{
		push rbp
		mov rbp, rsp
.Block1:
		mov r10, 5
		mov [rbp + -16], r10
		push r10
		mov r10, [rbp + -16]
		pop r11
		mov r10, [rbp + -16]
		push r10
		call fact
		add rsp, 8
		mov [rbp + -8], r10
		mov rax, num_format
		mov r10, [rbp + -8]
		push r10
		call _vprintf
		mov rax, 60
		mov rdi, 0
		syscall
;.EndBlock1
;}
fact:
;{
		push rbp
		mov rbp, rsp
.Block2:
		mov r10, [rbp + 16]
		mov r12, r10
		mov r10, 1
		mov r13, r10
		cmp r12, r13
		jg .Block3
		jmp .Block4
.Block3:
		push r10
		mov r10, 1
		push r10
		mov r10, [rbp + 16]
		pop r11
		sub r10, r11
		pop r11
		mov r10, 1
		push r10
		mov r10, [rbp + 16]
		pop r11
		sub r10, r11
		push r10
		call fact
		add rsp, 8
		mov [rbp + -8], r10
		mov r10, [rbp + -8]
		push r10
		mov r10, [rbp + 16]
		pop r11
		imul r10, r11
		sub rsp, 8
		mov rsp, rbp
		pop rbp
		ret
;.EndBlock3
.Block4:
		mov r10, 1
		sub rsp, 8
		mov rsp, rbp
		pop rbp
		ret
;.EndBlock4
;.EndBlock2
;}
;.EndBlock0
