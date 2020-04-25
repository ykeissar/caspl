section .data
    format: db "%d" , 10 ,0 

global assFunc
extern c_check_validity
extern printf
section .text

assFunc:                      ; 
    push ebp              	  ; save Base Pointer (bp) original value
    mov ebp, esp              ; reset EBP to the current ESP
    sub esp, 4                ; allocate space for local variable sum
    mov ebx, [ebp+8]          ; get first argument - x
    mov ecx, [ebp+12]         ; get second argument - y
    pushad                    ; backup registers
    pushfd                    ; backup EFLAGS
    push ecx                  ; push second arg - y
    push ebx                  ; push first arg - x
    call c_check_validity     ;
    mov [ebp-4], eax          ; saved returned value
    add esp, 8                ; clear stack from c_check_validity args
    popfd                     ; restore flags
    popad                     ; restore registers
    mov eax, [ebp-4]          ; restore return value to eax
    add eax, 0                ; eax = 0 => ZF=1, eax = 1 => ZF=0
    jz xIsGreater             ; x < y (ZF=1)
        neg ecx               ; maybe switch y = -y
    xIsGreater:
    add ebx, ecx              ; x = x + y
    pushad                    ; backup registers
    pushfd                    ; backup EFLAGS
    push ebx                  ; push printf second arg 
    push dword format         ; push printf first arg  
    call printf
    add esp, 8                ; delete printf args
    popfd                     ; restore flags
    popad                     ; restore registers
    mov esp, ebp 			  ; Restore caller state
    pop ebp 			      ; Restore caller state
    ret
    
           




    



