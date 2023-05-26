
GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _irq00Handler
GLOBAL _irq01Handler
GLOBAL _irq02Handler
GLOBAL _irq03Handler
GLOBAL _irq04Handler
GLOBAL _irq05Handler

GLOBAL _irq60Handler
GLOBAL _exception0Handler

GLOBAL _exception6Handler
EXTERN irqDispatcher
EXTERN exceptionDispatcher
EXTERN schedule

SECTION .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro

; se guarda el stack para poder devolverlo cuando ocurre una excepcion
%macro exceptionHandler 1

	pushState
	mov rdi, %1 ; pasaje de parametro
	mov rsi,rsp
	call exceptionDispatcher

	popState

	iretq

%endmacro

%macro pushStateNoRAX 0
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popStateNoRAX 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
%endmacro

; version para context switching, deberia ser activada cuando interrumpe el timer tick y cuando termina un proceso
%macro irqHandler 1
	pushState

	mov rdi,%1
	call irqDispatcher

	mov rdi, rsp
	call schedule    ;scheduler deberia estar en c y en este caso guardar en una tabla el valor del rsp viejo
	mov rsp, rax

	; signal pic EOI (End of Interrupt) para hardware
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro

; para syscalls, no se desea restaurar el RAX ya que el retorno de la syscall esta alli
%macro sirqHandler 1
	pushStateNoRAX

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popStateNoRAX
	iretq
%endmacro


_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di  ; ax = mascara de 16 bits
    out	0A1h,al
    pop     rbp
    retn


;8254 Timer (Timer Tick)
_irq00Handler:
	irqHandler 0
	;irqHandlerMaster 0

;Keyboard
_irq01Handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02Handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03Handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04Handler:
	irqHandlerMaster 4

;USB
_irq05Handler:
	irqHandlerMaster 5

;syscalls no bloqueantes
_irq60Handler:

	;se pasan 7 parametros ya que existe una syscall que recibe 6 parametros, debido al
	; corrimiento se tiene que pasar uno de esos parametros por stack
	push r9
	mov r9, r8
	mov r8,rcx
	mov rcx,rdx
	mov rdx,rsi
	mov rsi,rdi
	;muevo los parametros de handler para q al dispatcher le lleguen bien
	mov rdi,60h
	call irqDispatcher
	pop r9

	iretq


;syscalls bloqueantes, sospecho el por que no usamos push state y pop state
_birq60Handler:

	;se pasan 7 parametros ya que existe una syscall que recibe 6 parametros, debido al
	; corrimiento se tiene que pasar uno de esos parametros por stack
	push r9
	mov r9, r8
	mov r8,rcx
	mov rcx,rdx
	mov rdx,rsi
	mov rsi,rdi
	;muevo los parametros de handler para q al dispatcher le lleguen bien
	mov rdi,60h
	call irqDispatcher

	; interrupcion del timer

	pop r9

	iretq

;Zero Division Exception
_exception0Handler:
	exceptionHandler 0

;Invalid Op code Exception
_exception6Handler:
	exceptionHandler 6

haltcpu:
	cli
	hlt
	ret



SECTION .bss
	regist resq 17
