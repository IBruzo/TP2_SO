GLOBAL gettick
GLOBAL write
GLOBAL getchar
GLOBAL halt
GLOBAL getRegisters
GLOBAL getLastKey
GLOBAL putSquare
GLOBAL getTime
GLOBAL beep
GLOBAL sleep
GLOBAL clearkeybuffer
GLOBAL changelanguage
GLOBAL storeRegisters
GLOBAL memAccess
GLOBAL mAlloc
GLOBAL mFree
GLOBAL scrollUp
GLOBAL createProcess
GLOBAL getPid
GLOBAL increasePriority
GLOBAL decreasePriority
GLOBAL yield
GLOBAL kill
GLOBAL exit
GLOBAL semCreate
GLOBAL semOpen
GLOBAL semClose
GLOBAL semWait
GLOBAL semPost
GLOBAL waitPid
GLOBAL block
GLOBAL unblock
GLOBAL nice
GLOBAL changeInputFD
GLOBAL changeOutputFD
GLOBAL getInputFD
GLOBAL getOutputFD
GLOBAL openPipe
GLOBAL closePipe
GLOBAL ps
GLOBAL mem

section .text

%macro pushState 0
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
%endmacro

%macro sys_call 1
	pushState
	; corre todos los registros hacia atras para que se pueda poner el irq primero
	mov r9, r8
	mov r8, rcx
	mov rcx,rdx
    mov rdx,rsi
    mov rsi,rdi
	mov rdi, %1
	int 80h

	popState
	ret
%endmacro

;-----------------------------------------------;
;			nuestras syscalls					;
;												;
;-----------------------------------------------;
getchar:
	sys_call 0

write:
    sys_call 1

gettick:
	sys_call 2

halt:
	sys_call 3

putSquare:
	sys_call 4

getTime:
	sys_call 5

getLastKey:
	sys_call 6

getRegisters:
	sys_call 7

beep:
	sys_call 8

sleep:
	sys_call 9

clearkeybuffer:
	sys_call 10

changelanguage:
	sys_call 11

storeRegisters:
	sys_call 12

memAccess:
	sys_call 13

mAlloc:
	sys_call 14

mFree:
	sys_call 15

createProcess:
	sys_call 16

scrollUp:
	sys_call 17

getPid:
	sys_call 18

increasePriority:
	sys_call 19

decreasePriority:
	sys_call 20

yield:
	sys_call 21

kill:
	sys_call 22

exit:
	sys_call 23

semCreate:
	sys_call 24

semOpen:
	sys_call 25

semClose:
	sys_call 26

semWait:
	sys_call 27

semPost:
	sys_call 28

mem:
	sys_call 29

waitPid:
	sys_call 30

block:
	sys_call 31

unblock:
	sys_call 32

nice:
	sys_call 33

changeInputFD:
	sys_call 34

changeOutputFD:
	sys_call 35

getInputFD:
	sys_call 36

getOutputFD:
	sys_call 37

openPipe:
	sys_call 38

closePipe:
	sys_call 39
ps:
	sys_call 40