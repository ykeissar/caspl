.global c_checkValidity

assFunc:                      ;assFunc function does.....
    push ebp              		; save Base Pointer (bp) original value
    mov ebp, esp              ; reset EBP to the current ESP
    sub esp, 4                ; allocate space for local variable sum
    mov ebx, [ebp+8]          ; get first argument
    mov ecx, [ebp+12]         ; get second argument
    pushad                    ; backup registers
    pushfd                    ; backup EFLAGS
    push ebx                  ;maybe need to switch
    push ecx                  ;
    call c_checkValidity      ;
    mov [ebp-4], eax          ; saved returned value
    popfd                     ;
    popad                     ;
    mov eax, [ebp-4]          ; restore return value to eax
    add esp, 8                ;
    add eax, 0                ; eax = 0 => ZF=1, eax = 1 => ZF=0
    jz xIsGreater             ; x < y (ZF=1)
    neg ebx                   ; maybe switch y = -y
    xIsGreater:
      add ecx, ebx            ; x = x + y
    mov eax, ecx              ; place returned value where caller can see it
    add esp, 4 			          ; Restore caller state
    pop ebp 			            ; Restore caller state
    ret
    
           




    



