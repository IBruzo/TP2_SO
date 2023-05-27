GLOBAL cpuVendor
GLOBAL getTime
GLOBAL getkey
GLOBAL inb
GLOBAL outb
GLOBAL snapshot
GLOBAL buildDummyStack
GLOBAL getSP
section .text

cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

;recibe el modo y cual quiere q este de minutos horas etc
;devuelve en BCD el numero correspondiente a al hora minutos... etc
getTime:
	push rbp
	mov rbp, rsp

	push rbx

	mov eax,edi
	out 70h,al
	in al,71h

	pop rbx

	mov rsp, rbp
	pop rbp
	ret
;

inb:
	push rbp
	mov rbp, rsp
	mov rdx,rdi	;puerto
	in al,dx	;valor del puerto
	leave
	ret

outb:
	push rbp
	mov rbp, rsp
	mov rax, rsi ;valor
	mov rdx, rdi ;puerto
	out dx, al
	leave
	ret

; retorna un puntero a una zona de memoria donde se encuentra el estado de todo los registros
snapshot:

	; es inevitable que algunos registros tengan valores fijos porque para llegar aca
	; hay que invitablemente usar registros, entonces no es un snapshot real

	mov [regsBuffer], rax
	mov [regsBuffer+8], rbx
	mov [regsBuffer+16], rdx
	mov [regsBuffer+24], rcx
	mov [regsBuffer+32], rsi
	mov [regsBuffer+40], rdi
	mov [regsBuffer+48], rbp
	mov [regsBuffer+56], rsp
	mov [regsBuffer+64], r8
	mov [regsBuffer+72], r9
	mov [regsBuffer+80], r10
	mov [regsBuffer+88], r11
	mov [regsBuffer+96], r12
	mov [regsBuffer+104], r13
	mov [regsBuffer+112], r14
	mov [regsBuffer+120], r15

	mov rax, regsBuffer

	ret
getSP:
	mov rax, rsp
	ret

buildDummyStack:

	;PARAMETROS DE BUILDDUMMYSTACK:

	;RDI -> Stack Pointer/Fin del bloque
	;RSI -> Puntero a funcion
	;RDX -> #Argumentos, int
	;RCX -> Argumentos, Array de Strings

	;push rbp
    ;mov rbp, rsp

	mov rbp, rdi
    mov rsp, rdi ; stack base
    and rsp, -16
    push 0x0
    push rdi
    push 0x202
    push 0x8
    push rsi


	;mov rbp, rdi		;Muevo el base pointer al final de la pagina
	;mov rsp, rdi
	;push 0				;Align
	;push 0				;Stack Segment
	;push rdi			;RSP cuando ocurrio la interrupcion, como es un proceso nuevo, es el base pointer
	;push 0x202			;RFLAGS
	;push 0x8			;Code Segment
	;push rsi			;Entry Point, puntero a funcion			<- INVALID OP CODE ???

	;PARAMETROS DEL NUEVO PROCESO:

	;RDI -> ARGC
	;RSI -> ARGV

	push 0				;RAX
	push 0				;RBX
	push 0				;RCX
	push 0				;RDX
	push 0				;RBP
	push 0				;RDI, ARGC
	push 0				;RSI, ARGV
	push 0				;R8
	push 0				;R9
	push 0				;R10
	push 0				;R11
	push 0				;R12
	push 0				;R13
	push 0				;R14
	push 0				;R15

	_sti
	int 20h				;Llamo a la interrupcion del timer

	ret


section .bss
regsBuffer resb 128


