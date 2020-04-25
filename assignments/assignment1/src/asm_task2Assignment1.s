section	.rodata			; we define (global) read-only variables in .rodata section
	format_string: db "%s", 10, 0	; format string

section .bss			; we define (global) uninitialized variables in .bss section
	an: resb 12		; enough to store integer in [-2,147,483,648 (-2^31) : 2,147,483,647 (2^31-1)]

section .text
	global convertor
	extern printf

convertor:
	push ebp
	mov ebp, esp	
	pushad			

	mov ecx, dword [ebp+8]		; get function argument (pointer to string)

	sub esp, 4                  ; allocate space in stack for pointer to the first string char
	mov [ebp-4], esp            ; save the pointer to the first string char
	mov eax, 0					; fill eax with 0
	startAsciiToDec:
		mov al, [ecx]           ; get [ecx] first byte
		add ecx, 1				; ecx++
		sub al, 48				; convert al from ascii to dec
		push eax				; push the result to stack
		sub al, -38				; check if the byte eql 10
	jnz startAsciiToDec
	pop ebx                     ; removing first digit from stack (\n)
	mov ecx, 0                  ; the sum
	mov ebx, 1                  ; the multiplier
	sumEax:
		pop eax                 ; get next digit
		mul ebx                 ; multiply by ebx = 10^k
		add ecx, eax            ; add current number to sum
		mov eax, 10             ; 
		mul ebx                 ; ebx = ebx*10
		mov ebx, eax            ;
		mov edx, esp            ;
		sub edx, [ebp-4]        ; check if digits left
	jnz sumEax                  

	mov eax, ecx                ; put sum in eax
	mov ebx, 16                 ; the divisor

	mov [ebp-4], esp            ; save the pointer to the first string char
	divisitonStart:
		div ebx                 ; div eax by 16
		add edx, 48             ; add 48 to the remainder to convert to ascii (48-63) 
		mov ecx, edx            ;
		sub ecx, 58             ; check if 0-9 or A-F (48-57) or (58-63)
		js ifDigit
			add edx, 7          ; add 7 if A-F
		ifDigit:  
			push edx            ; put char in stack
			mov edx, 0			; to make division correct
			add eax, 0          ; eax = 0 if finished
	jnz divisitonStart

	mov edx, an					; save pointer to the first string char in edx
	afterDevision:
		pop ecx					; get next char
		mov [edx], ecx			; put in returned value
		add edx, 1				; move pointer to the next byte
		
		mov ebx, [ebp-4]        ; check if got to end of digits
		sub ebx ,esp            ;
	jnz afterDevision

	push an						; call printf with 2 arguments
	push format_string			; pointer to str and pointer to format string
	call printf
	add esp, 12					; clean up stack after call

	popad
	mov esp, ebp	
	pop ebp
	ret	