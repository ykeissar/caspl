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
    call generateRandF
    mov [ebx+%1],eax
    popad
    popfd
%endmacro

section .data

section .bss
    N: resb 4
    R: resb 4
    K: resb 4
    d: rest 1
    seed: resb 4

    CORS: resd 1                            ; co-routines aray
    CURR: resd 1                            ; current co-routine
    SPT: resd 1                             ; temporary stack pointer
    SP_MAIN: resd 1                          ; stack pointer of main

section .rodata
    format_int: db "%d",0
    format_float: db "%f",0

    CODEP equ 0                             ; co-routine struct
    SPP equ 4
    COID equ 8
    CORD_X equ 14
    CORD_Y equ 24
    DIRECT equ 34
    SPEED equ 44
    SCORE equ 54                            ;total co-routine size - 62

    COR_SIZE equ 58
    STK_SIZE equ 16*1024                     ;16 Kb

    WRAPING_ANG: dd 360
    WRAPING_BOARD: dd 100


section .text
    global main
    extern sscanf
    extern printf
    extern malloc
    extern free

    main:
        push ebp
        mov ebp,esp
        sub esp,4

        mov eax, dword [ebp+8]              ; argc     
        mov ebx, dword [ebp+12]             ; **argv

        add ebx,4                           ; arg[0] = "ass3", skip
        handleArgs:
            mov edx,[ebx]                       ;
            handleArg N

            add ebx,4
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
            handleArg edx,format_int,seed

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

    droneFunc:
        sub esp,4
        finit
        mov ebx,[CURR]                          ;

        generateAndSetAngel:
            pushad  
            push dword 120
            generateRandF        
            mov dword[ebp-4],eax                         ; generate d-angle [0,120]
            popad

            sub word[ebp-4],60                          ; set to [-60,60]
            fld tbyte[ebx+DIRECT]
            fld dword[ebp-4]
            faddp
            fstp tbyte[ebx+DIRECT]
            
            cmp tbyte[ebx+DIRECT], 360                           ;verify works
            jna notGreaterAngle 
                fld dword [WRAPING_ANG]
                fld tbyte [ebx+DIRECT]
                fsubp
                fstp tbyte [ebx+DIRECT]
            notGreaterAngle:
            
            cmp 0, tbyte[ebx+DIRECT]                           ;verify works
            jna notLessAngle 
                fld dword [WRAPING_ANG]
                fld tbyte [ebx+DIRECT]
                faddp
                fstp tbyte [ebx+DIRECT]   
            notLessAngle:


        free
        add esp,4

    generateRandF:                          ; generate num between 0-X


