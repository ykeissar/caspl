%macro handleArg 3
    pushad
    push %3
    push %2
    push %1
    call sscanf
    add esp,12
    popad
%endmacro

%macro setRand 2
    pushfd
    pushad
    push dword %2
    call generateRand
    add esp,4
    finit
    fld qword[numGenerated]
    fstp qword [ebx+%1]
    ffree
    popad
    popfd
%endmacro

%macro pushFloat 1
    finit
    fld qword[%1]
    sub esp,8
    fstp qword [esp]
    ffree
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
    numGenerated: resd 2

    CORS: resd 1                            ; co-routines aray
    CURR: resd 1                            ; current co-routine
    SPT: resd 1                             ; temporary stack pointer
    SP_MAIN: resd 1                          ; stack pointer of main

section .rodata
    format_int: db "%d",0
    format_float: db "%f",10,0
    format_short: db "%hd",0
    formatDebugNum: db "Generated the number: %f",10,0 
    formatDebugNumSeed: db "Next Seed: %hd",10,0 
    format_printCor: db "%d, %f, %f, %f, %f, %d",10,0

    CODEP equ 0                             ; co-routine struct
    SPP equ 4
    COID equ 8
    CORD_X equ 14
    CORD_Y equ 22
    DIRECT equ 30
    SPEED equ 38
    SCORE equ 46                          ;total co-routine size - 50 

    COR_SIZE equ 50
    STK_SIZE equ 16*1024                     ;16 Kb

    WRAPING_ANG: dd 360
    WRAPING_BOARD: dd 100
    MAX_SPEED: dd 100
    MAX_INT16: dw 0x7fff

    TARGET_X: dd 8
    TARGET_Y: dd 8

section .text
    global main
    extern sscanf
    extern printf
    extern fprintf
    extern stdout
    extern malloc
    extern free


    main:
        push ebp
        mov ebp,esp
        sub esp,4

        mov ebx, dword[ebp+12]

        add ebx,4
        mov edx,[ebx]
        handleArg edx,format_short,seed

        pushad
        push COR_SIZE
        call malloc
        mov [CORS],eax
        add esp,4
        popad

        pushad
        push dword 0
        call initCoState
        add esp,4
        popad

        pushad
        push dword[CORS]
        call printCor
        add esp,4
        popad

        mov ebx,[CORS]
        call do_resume

        mov esp,ebp
        pop ebp
        ret 
         
    other_main:
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

        InitTaget:
            setRand TARGET_X,100
            setRand TARGET_Y,100

        mov esp,ebp
        pop ebp
        ret


    initCoState:
        push ebp
        mov ebp,esp

        mov ebx,[ebp+8]                         ; cor id
        mov eax,COR_SIZE                      ;
        mul ebx
        mov ebx,[CORS]
        add eax,ebx
        mov ebx,eax    ; ebx = COi struct pointer in CORS  //verify [] 
        
        add ebx,CODEP
        mov dword[ebx],droneFunc 
        
        pushad
        push dword STK_SIZE
        call malloc                             ; allocate stack to coroutine
        add esp,4
        add eax,STK_SIZE
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

        setRand CORD_X,100
        setRand CORD_Y,100
        setRand DIRECT,120
        setRand SPEED,100

        mov [ebx+SCORE], dword 0  
        mov esp,ebp
        pop ebp
        ret

    droneFunc:          ;[ebp-8] = rawAngle, [ebp-16]= rawSpeed
        sub esp,16          
        finit
        mov ebx,[CURR]                          ;

        generateNewFields:
            generateAngle:
                pushad  
                push dword 120             ;120 = upper bound = 100
                call generateRand               ; generate d-angle [0,120]
                add esp,4
                popad

                fld qword[numGenerated]         ; set to [-60,60]
                fPushInt 60
                fsubp
                fld qword[ebx+DIRECT]       
                faddp
                fstp qword[ebp-8]
                
            generateSpeed: 
                pushad 
                push dword 20             
                call generateRand        
                add esp,4
                popad


                fld qword[numGenerated] 
                fPushInt 10
                fsubp                        ; set to [-10,10]

                fld qword[ebx+SPEED]         ;SPEED=offset to relevant field
                faddp
                fstp qword[ebp-16]
                ffree
            DroneMovement:                   ;(x1,y1) -> (x1+v*cosa, y1+v*sina)
                finit
                computingX:
                    fld qword[ebx+DIRECT]           ;transfering angke to radians representation
                    fldpi
                    fmulp
                    fPushInt 180
                    fdivp
                    fcos                            ; result not necceserly in 32 bits 
                    fld qword[ebx+SPEED]
                    fmulp 
                    fld qword[ebx+CORD_X]
                    faddp
                    
                    fst qword[ebx+CORD_X]

                    fPushInt [WRAPING_BOARD]                    ; check if pass limit
                    fcomp
                    jae notAbove100X                     ;jumps if st(0)<=st(1)
                        fPushInt [WRAPING_BOARD] 
                        fsubp 
                    notAbove100X:
                    fPushInt 0
                    fcomp 
                    jae notBelowX0
                        fPushInt [WRAPING_BOARD]
                        faddp
                    notBelowX0:
                    fstp qword[ebx+CORD_X]
                computingY:             
                    fld qword[ebx+DIRECT]           ;transfering angke to radians representation
                    fldpi
                    fmulp
                    fPushInt 180
                    fdivp
                    fsin                            ; result not necceserly in 32 bits 
                    fld qword[ebx+SPEED]
                    fmulp 
                    fld qword[ebx+CORD_Y]
                    faddp
                    
                    fst qword[ebx+CORD_Y]

                    fPushInt [WRAPING_BOARD]                    ; check if pass limit
                    fcomp
                    jae notAbove100Y                     ;jumps if st(0)<=st(1)
                        fPushInt [WRAPING_BOARD] 
                        fsubp 
                    notAbove100Y:
                    fPushInt 0
                    fcomp 
                    jae notBelowY0
                        fPushInt [WRAPING_BOARD]
                        faddp
                    notBelowY0:
                    fstp qword[ebx+CORD_Y]
         
            setNewAngle:
                fld qword [ebp-8]                   ;load raw new angle
                fPushInt dword[WRAPING_ANG] 
                fcomp
                jae notGreaterAngle
                    fPushInt dword[WRAPING_ANG]
                    fsubp
                notGreaterAngle:
                fPushInt dword 0
                fcomp
                jbe notNegAngle
                    fPushInt dword[WRAPING_ANG]
                    faddp
                notNegAngle:
                fstp qword [ebx+DIRECT]

            setNewSpeed:
                fld qword [ebp-16]                   ;load raw new speed
                fPushInt dword[MAX_SPEED] 
                fcomp
                jae notGreaterSpeed
                    fPushInt dword[MAX_SPEED]
                notGreaterSpeed:
                fPushInt dword 0
                fcomp
                jae notNegSpeed
                    fPushInt dword 0
                notNegSpeed:
                fstp qword [ebx+SPEED]

        MayDestroy:
            
            DestroyTarget:
                add dword[ebx+SCORE],1

                setRand TARGET_X,100
                setRand TARGET_Y,100

                jmp generateNewFields

        pushad
        push dword[CORS]
        call printCor
        add esp,4
        popad
        
        
        ffree
        add esp,16

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
        fabs
        fstp qword [numGenerated] 
        ffree

        mov esp,ebp
        pop ebp
        ret

    printCor:
        push ebp
        mov ebp,esp

        mov ebx,[ebp+8]             ;ebx=pointer to curr cor         
        pushad

        push dword[ebx+SCORE]         
        pushFloat ebx+SPEED
        pushFloat ebx+DIRECT
        pushFloat ebx+CORD_Y
        pushFloat ebx+CORD_X
        push dword[EBX+COID]
        push format_printCor
        call printf
        
        add esp,44
        popad
        pop ebp
        ret

    resume: ; save state of current co-routine
        pushfd
        pushad
        mov EDX, [CURR]
        mov [EDX+SPP], ESP ; save current ESP
    do_resume: ; load ESP for resumed co-routine
        mov ESP, [EBX+SPP]
        mov [CURR], EBX
        popad ; restore resumed co-routine state
        popfd
        ret ; "return" to resumed co-routine