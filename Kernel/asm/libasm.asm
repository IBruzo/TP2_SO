GLOBAL cpuVendor
GLOBAL getTime
GLOBAL getkey
GLOBAL inb
GLOBAL outb
GLOBAL snapshot
GLOBAL buildDummyStack
GLOBAL getSP
GLOBAL forceTick
GLOBAL _xchg
section .text


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

; Lee la hora haciendo un comando sobre el RTC, devuelve en formato BCD
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

; Funcion utilizado con recurrencia, para retomar tareas de scheduling luego
; de llegar a bloqueos o exits
forceTick:
	sti
	int 20h
	ret

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

; Retorna un Puntero a una Zona de Memoria del Kernel donde se encuentra el Snapshot
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

; Constructora de Stack Falso, se cambio a una version en C por recomendacion de los profesores
; pero por respeto se mantuvo
buildDummyStack:
	;PARAMETROS DE BUILDDUMMYSTACK:
	;RDI -> Stack Pointer/Fin del bloque
	;RSI -> Puntero a Funcion
	;RDX -> #Argumentos, int
	;RCX -> Argumentos, Array de Strings

	mov rbp, rdi		; RBP = Fin del Bloque
    mov rsp, rdi 		; RSP = Fin del Bloque
    and rsp, -16		; Alineamiento
    push 0x0			; Stack Segment
    push rdi			; Como es un Proceso Nuevo tiene un Stack vacio => RSP = RBP = Fin del Bloque
    push 0x202			; Flags
    push 0x8			; Code Segment
    push rsi			; Entry Point = Puntero a Funcion

	push 0				;RAX
	push 0				;RBX
	push 0				;RCX
	push 0				;RDX
	push 0				;RBP
	push rdx			;RDI, ARGC del Proceso Nuevo
	push rcx			;RSI, ARGV del Proceso Nuevo
	push 0				;R8
	push 0				;R9
	push 0				;R10
	push 0				;R11
	push 0				;R12
	push 0				;R13
	push 0				;R14
	push 0				;R15

	int 20h				;Fuerzo una interrupcion del timer

	ret

; Atomicidad para le manejo interno de los semaforos
_xchg:
	mov rax, rsi
	xchg [rdi], eax 
	ret

section .bss
regsBuffer resb 128


