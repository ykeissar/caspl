%macro handleArg 3
    pushad
    push %3
    push %2
    push %1
    call sscanf
    add esp,12
    popad
%endmacro

%macro setRand 1
    pushfd
    pushad
    call generateRand
    mov [ebx+%1],eax
    popad
    popfd
%endmacro

%macro fPushInt 1
    push dword %1
    fild dword[esp]
    pop edi
%endmacro

%macro isolateBit 2
    pushad
    mov ecx,0
    mov edx,0

    mov cx, %1
    mov dx, word[seed]
    and cx,dx
    shr cx,%2
    mov word[ebp-4],cx

    popad
%endmacro

section .data
    
section .bss
    N: resb 4
    R: resb 4
    K: resb 4
    d: rest 1
    seed: resb 2

    CORS: resd 1                            ; co-routines aray
    CURR: resd 1                            ; current co-routine
    SPT: resd 1                             ; temporary stack pointer
    SP_MAIN: resd 1                          ; stack pointer of main

section .rodata
    format_int: db "%d",0
    format_float: db "%f",0
    format_short: db "%hd",0
    formatDebugNum: db "Generated the number: %f",10,0 
    formatDebugNumSeed: db "Next Seed: %hd",10,0 

    CODEP equ 0                             ; co-routine struct
    SPP equ 4
    COID equ 8
    CORD_X equ 14
    CORD_Y equ 18
    DIRECT equ 22
    SPEED equ 26
    SCORE equ 30                            ;total co-routine size - 34

    COR_SIZE equ 34
    STK_SIZE equ 16*1024                     ;16 Kb

    WRAPING_ANG: dd 360
    WRAPING_BOARD: dd 100
    MAX_INT16: dw 0xffff


section .text
    global main
    extern sscanf
    extern printf
    extern fprintf
    extern stdout
    extern malloc
    extern free


    main:                   ;tests
        push ebp
        mov ebp,esp

        mov edx,0
        mov ebx, dword[ebp+12]
        add ebx,4
        mov ecx,dword[ebx]

        handleArg ecx,format_short,seed

        mov ecx,0
        mov ecx,5

        startTestLoop:
            pushad
            mov ecx, 0
            mov cx, word[seed] 
            push ecx
            push dword formatDebugNumSeed
            call printf
            add esp, 8
            popad

            
            pushad
            mov ecx, 0
            mov cx, [MAX_INT16]
            push ecx
            call generateRand
            mov dword[ebp-4],eax
            add esp,4
            popad

            pushad
            push dword[ebp-4]
            push dword formatDebugNum
            call printf
            add esp, 8
            popad
            
            sub ecx,1
            cmp ecx,0
            jnz startTestLoop
        
        mov esp,ebp
        pop ebp
        ret

    normal_main:
        push ebp
        mov ebp,esp
        sub esp,4

        mov eax, dword [ebp+8]              ; argc     
        mov ebx, dword [ebp+12]             ; **argv

        add ebx,4                           ; arg[0] = "ass3", skip
        handleArgs:
            mov edx,[ebx]                       ;
            handleArg edx,format_int,N
    
            add ebx,4
            mov edx,[ebx]                       ;
            handleArg edx,format_int,R

            add ebx,4
            mov edx,[ebx]
            handleArg edx,format_int,K

            add ebx,4
            mov edx,[ebx]
            handleArg edx,format_float,d

            add ebx,4
            mov edx,[ebx]
            handleArg edx,format_short,seed

        initCorsArray:
            mov eax,[N]                             
            mov ebx,[COR_SIZE]
            mul ebx
            
            pushad
            push eax
            call malloc
            add esp,4
            mov [CORS],eax
            popad

            mov eax,[N]                 
            sub eax,1                
            mov ecx,0
            initCorsLoop:
                cmp ecx,eax
                jz endInitCorsLoop

                pushad
                push ecx
                call initCoState
                add esp,4
                popad
                inc ecx
            endInitCorsLoop:

        mov esp,ebp
        pop ebp
        ret

    initCoState:
        push ebp
        mov ebp,esp

        mov ebx,[ebp+8]                         ; cor id
        mov eax,[COR_SIZE]                      ;
        mul ebx
        mov ebx, [eax + CORS]                   ; ebx = COi struct pointer in CORS  //verify [] 
        
        mov dword [ebx+CODEP],droneFunc 
        
        pushad
        push dword[STK_SIZE]
        call malloc                             ; allocate stack to coroutine
        add esp,4
        add eax,[STK_SIZE]
        mov [ebx+SPP],eax
        popad

        mov [SPT],esp
        mov esp,[ebx+SPP]
        push dword[ebx+CODEP]
        pushfd                                  ;init cor stack state
        pushad
        mov [ebx+SPP],esp
        mov esp,[SPT]
        
        mov eax,[ebp+8]
        mov [ebx+COID],eax

        setRand CORD_X
        setRand CORD_Y
        setRand DIRECT
        setRand SPEED

        mov [ebx+SCORE], dword 0  
        mov esp,ebp
        pop ebp
        ret

    droneFunc:          ;[ebp-8] = rawAngle, [ebp-12]= rawSpeed
        sub esp,8
        finit
        mov ebx,[CURR]                          ;

        generateNewFields:
        generateAngle:
            pushad  
            push dword 120             ;120 = upper bound = 100
            call generateRand        
            mov dword[ebp-4],eax       ; generate d-angle [0,120]
            add esp,4
            popad

            fld dword[ebp-4]         ; set to [-60,60]
            fPushInt 60
            fsubp
            fstp dword[ebp-4]
            fld dword[ebx+DIRECT]       
            fld dword[ebp-4]
            faddp
            fstp dword[ebp-8]
            
        generateSpeed: 
            pushad 
            push dword 20             
            call generateRand        
            mov dword[ebp-4],eax         ; generate d-speed [0,20]
            add esp,4
            popad


            fld dword[ebp-4]
            fPushInt 10
            fsubp                        ; set to [-10,10]
            fstp dword[ebp-4]

            fld dword[ebx+SPEED]         ;SPEED=offset to relevant field
            fld dword[ebp-4]
            faddp
            fstp dword[ebp-12]
        
        DroneMovement:                   ;(x1,y1) -> (x1+v*cosa, y1+v*sina)
        finit
        ;computingX
        fld dword[ebx+DIRECT]           ;transfering angke to radians representation
        fldpi
        fmulp
        fPushInt 180
        fdivp
        fcos                            ; result not necceserly in 32 bits 
        fld dword[ebx+SPEED]
        fmulp 
        fld dword[ebx+CORD_X]
        faddp
        
        fst dword[ebx+CORD_X]           ;chek if abov range
        fPushInt 100
        fcom
        jbe notAbove100X                     ;jumps if st(0)<=st(1)
        fsubp 
        notAbove100X:
            fld dword[ebx+CORD_X]
            fPushInt 0
            fcomp 
            jae notBelowX0
            fPushInt 100
            faddp
        notBelowX0:
            fstp dword[ebx+CORD_X]
        ;computingY              
        fld dword[ebx+DIRECT]           ;transfering angke to radians representation
        fldpi
        fmulp
        fPushInt 180
        fdivp
        fcos                            ; result not necceserly in 32 bits 
        fld dword[ebx+SPEED]
        fmulp 
        fld dword[ebx+CORD_Y]
        faddp
        
        fst dword[ebx+CORD_Y]           ;chek if abov range
        fPushInt 100
        fsin
        jbe notAbove100Y                     ;jumps if st(0)<=st(1)
        fsubp 
        notAbove100Y:
            fld dword[ebx+CORD_Y]
            fPushInt 0
            fcomp 
            jae notBelowY0
            fPushInt 100
            faddp
        notBelowY0:
            fstp dword[ebx+CORD_Y]
        ffree
        add esp,8
        cmp dword[ebp-8], 360              ;verify works
        jna notGreaterAngle  
            fld dword [WRAPING_ANG]
            fld dword [ebp-8]
            fsubp
            fstp dword [ebx+DIRECT]
        notGreaterAngle:
        
        cmp dword[ebp-8],0       ;lower bound=0                           ;verify works
        jna notLessAngle 
            fld dword [WRAPING_ANG]
            fld dword [ebp-8]
            faddp
            fstp dword [ebx+DIRECT]   
        notLessAngle:

        generateAndSetCords:
        
        
        ffree
        add esp,4

    generateRand:                         ;operand: upper bound ; generate num between 0-X
        push ebp
        mov ebp,esp
        sub esp,4

        mov ecx,0        
        mov edx,0
        mov dx,[seed]
    
        isolateBit 1,0            ; 16th bit
        mov cx,[ebp-4]
        
        isolateBit 4,2             ;14th bit
        xor cx,word[ebp-4]
        
        isolateBit 8,3             ;13th bit
        xor cx, word[ebp-4]

        isolateBit 32,5            ;11th bit
        xor cx, word[ebp-4]

        
        cmp cx, 0
        jnz notZero
        mov cx,1
        not cx
        and dx,cx                ;masking with and 11111..10            ;
        jmp afterLFSR
        notZero:
            or cx,dx             ;masking with or 0000...01
        afterLFSR:
        ror dx,1
        mov word[seed],dx

        finit                          ; scaled_num = floatingP of(x/maxint) * [ebp+8]
        fild word[seed]    
        fild word[MAX_INT16]
        fdivp
        fild word[ebp+8]                ;CHECK if theres a conflict between input-usingfunc
        fmulp
        fstp dword [ebp-4] 
        ffree
        mov eax, [ebp-4]

        mov esp,ebp
        pop ebp
        ret
