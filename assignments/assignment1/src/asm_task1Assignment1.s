section .data
    format: db "%d" , 10 ,0 

global assFunc
extern c_check_validity
extern printf
section .text

assFunc:                      ;assFunc function does.....
    push ebp              		; save Base Pointer (bp) original value
    mov ebp, esp              ; reset EBP to the current ESP
    sub esp, 4                ; allocate space for local variable sum
    mov ebx, [ebp+8]          ; get first argument
    mov ecx, [ebp+12]         ; get second argument
    pushad                    ; backup registers
    pushfd                    ; backup EFLAGS
    push ecx                  ;maybe need to switch
    push ebx                  ;
    call c_check_validity     ;
    mov [ebp-4], eax          ; saved returned value
    add esp, 8                ;
    popfd                     ;
    popad                     ;
    mov eax, [ebp-4]          ; restore return value to eax
    add eax, 0                ; eax = 0 => ZF=1, eax = 1 => ZF=0
    jz xIsGreater             ; x < y (ZF=1)
    neg ecx                   ; maybe switch y = -y
    xIsGreater:
    add ebx, ecx              ; x = x + y
    mov eax, ebx              ; place returned value where caller can see it
    push eax 
    push dword format
    call printf
    mov esp, ebp 			          ; Restore caller state
    pop ebp 			            ; Restore caller state
    ret
    
           




    



