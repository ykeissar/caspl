;Macro start

%define MAX_INPUT_LENGTH 80

%macro getUserInput 0
    myPrintf stdout,default_msg, format_string
    pushad
    pushfd
    push dword [stdin]                ; 3rd arg - FILE pointer
    push MAX_INPUT_LENGTH             ; 2nd arg - string length
    push dword inputString            ; 1st arg - maybe need to clean inputString
    call fgets                        ;
    myFlush stdin                   
    add esp, 12
    popfd
    popad
%endmacro

%macro myPrintf 3+
    pushad
    pushfd
    mov eax, %1

    push dword %2             ; 3rd arg, string pointer
    push dword %3             ; 2nd arg, format string
    push dword [eax]
    call fprintf     
    myFlush %1
    add esp, 12
    popfd
    popad
%endmacro

%macro startFunc 1
    push ebp
    mov ebp, esp
    sub esp, %1
    pushad
    pushfd
%endmacro

%macro endFunc 1
    popfd
    popad
    add esp, %1
    pop ebp
    ret
%endmacro

%macro myFlush 1
    pushad
    pushfd
    mov eax, %1
    push dword [eax]                     ; 1st arg, FILE pointer
    call fflush
    add esp, 4
    popfd
    popad
%endmacro    

%macro stackSizeCheck 1
    pushad
    mov eax, %1
    mov ecx, 4
    mul ecx
    mov ebx, [opEbp]
    add eax, ebx
    cmp [opEsp], eax
    popad
%endmacro

%macro debugPrint 1
    pushad
    mov eax,0
    mov al,[debug]
    cmp eax,0
    jz %%dontPrint
    myPrintf stderr, format_string, %1
    %%dontPrint:
    popad
%endmacro

;Macro end

section .data
    debug: db 0
    stack_size: db 5
section .rodata
    format_string: db "%s", 0	                                                ; format string
    format_hexa2: db "%02X", 0                                                  ; format num
    format_hexa1: db "%X", 0                                                    ; format num

    new_line: db 10,0                                                           ; 
    default_msg: db "calc: ", 0                                                 ;
    debug_on: db "Debug mode is ON!",0                                          ;
    sum_msg: db "Sum result is: ",0                                             ;

    sof_error: db "Error: Operand Stack Overflow",10,0 ;
    empty_error: db "Error: Insufficient Number of Arguments on Stack",10,0     ;
    stack_size_error: db "Error: Stack size must be larger than 2",10,0         ;

section .bss
    inputString: resb MAX_INPUT_LENGTH        ; max input size is 80 chars
    stackPointer: resd 1                      ;
    opEsp: resd 1                             ;
    opEbp: resd 1                             ;
    
section .text
  align 16
  global main
  extern printf
  extern fprintf 
  extern fflush
  extern malloc 
  extern calloc 
  extern free 
  extern getchar 
  extern fgets
  extern stdin
  extern stdout
  extern stderr
;------------------------main-----------------------
main:
    push ebp                        ;
    mov ebp, esp                    ;
    sub esp, 8                      ; local var [ebp-4]

    mov eax, dword [ebp+8]         ; argc     
    mov ebx, dword [ebp+12]        ; **argv
    cmp eax,2
    js afterArgsHandleing
    
    Args:
        add ebx,4
        mov edx, [ebx]              ; pointer to string (args of argv)
        mov cl, byte[edx]              ; content of string
        cmp cl, 45                            ; - = 45 , d =100
        jnz notDebug
        mov cl, byte[edx+1]
        cmp cl, 100
        jnz notDebug
        mov [debug],byte 1
        debugPrint debug_on
        myPrintf stdout,new_line,format_string
        jmp endCurrArg
        
        notDebug:

            pushad
            mov ecx,0
            mov cl, [edx+1]
            push ecx 
            mov cl,[edx]
            push ecx 
            
            call convertToHexa
            add esp,8
            mov [stack_size], al
            popad 

        endCurrArg:
            sub eax,1
            cmp eax,1
            jnz Args

    afterArgsHandleing:
    
    verifyStackSize:
        mov eax, [stack_size]
        cmp eax, 3
    js stackSizeEror

    pushad
    mov eax, [stack_size]
    mov ecx, 4
    mul ecx
    push eax
    call malloc                     ; allocate stack mem
    mov [stackPointer], eax         ; set stack pointer
    add esp,4
    popad

    mov edx, [stackPointer]         ; 
    mov [opEsp],edx               ; set operand stack esp
    mov [opEbp], edx               ; set operand stack ebp

    mainLoop:
        getUserInput                ; print msg and receive input
    
    ifQuit:
        mov al, byte [inputString]
        cmp al, 113                 ; if inputString[0] == 'q'
        jz endMainLoop              ; endloop

    ifPopPrint:
        mov al, byte [inputString]
        cmp al, 112
        jnz ifDuplicate
        
        pushad
        call popOp
        mov [ebp-4],eax             ; get *Link to eax
        popad

        mov ecx, [ebp-4]
        cmp ecx, 0                   ; check if NULL was returned from pop
        jz mainLoop

        pushad
        push stdout
        push dword [ebp-4]
        call printNumList
        add esp,8
        popad

        jmp mainLoop  

    ifDuplicate:
        mov al,byte [inputString]
        cmp al, 100                     ; if al == 'd'
        jnz ifAdd
        
        pushad
        call peekOp                     ; put returned *Link in [ebp-4]
        mov [ebp-4],eax
        popad
        
        pushad
        push dword [ebp-4]               ; push *Link
        call copyNumList
        mov dword[ebp-4], eax
        add esp,4
        popad

        mov ecx, [ebp-4]
        cmp ecx, 0                   ; check if NULL was returned from pop
        jz mainLoop

        pushad
        push dword[ebp-4]    
        call pushOp
        add esp,4
        popad
        jmp mainLoop

    ifAdd:
        mov al, byte [inputString]
        cmp al, 43
        jnz ifN

        stackSizeCheck 2
        js notEnughtOps

        pushad
        call popOp
        mov [ebp-4],eax
        popad

        pushad
        call popOp
        mov [ebp-8],eax
        popad

        pushad
        push dword[ebp-4]
        push dword[ebp-8]
        call sumLinks
        mov [ebp-4],eax
        add esp,8
        popad

        debugPrint sum_msg
        
        pushad
        mov eax,0
        mov al,[debug]
        cmp eax,0
        jz skipPrint
        push stderr
        push dword [ebp-4]
        call printNumList                               ; print number summed if debug
        add esp,8
        skipPrint:
        popad

        pushad
        push dword[ebp-4]
        call pushOp
        add esp,4
        popad

        jmp mainLoop

    ifN:
        mov al, byte [inputString]
        cmp al, 110
        jnz ifNumber
        
        pushad
        call popOp
        mov [ebp-4],eax             ; get *Link to eax
        popad

        

        jmp mainLoop

    ifNumber:
        pushad
        push inputString            ; 1st arg createList - string pointer
        call createNumListFromStr
        mov [ebp-4],eax             ; save returned pointer to numList
        add esp,4
        popad    

        pushad
        push dword[ebp-4]           ; 1st arg pushOp - *numList
        call pushOp
        add esp,4
        popad

        jmp mainLoop

    notEnughtOps:
        myPrintf stderr, empty_error, format_string

    jmp mainLoop
    stackSizeEror:
        myPrintf stderr, stack_size_error,format_string    
    endMainLoop:
    mov esp,ebp
    pop ebp
    ret

;------------------------createNumListFromStr-----------------------
createNumListFromStr:               ; create linked number from string
    push ebp                        ;
    mov ebp, esp                    ;
    mov ebx,0
    mov ecx,0

    sub esp, 8                      ; local variable for each created link [ebp-4] and list [ebp-8] 
    mov edx, [ebp+8]                ; char* string
    mov [ebp-8],dword 0

    pushad
    call clearLeadingZeros
    popad
    
    pushad
    call ShiftIfOdd
    popad

    startLoop:
        mov bl,byte[edx]                    ;  [edx] = []...'0''1''\0'] | 11\0
        cmp bl,10                           ; ['0','1','2','3','\n','\0']
        jz endLoop

        pushad                              ;
        push dword [edx]                    ;         
        call createLink
        mov [ebp-4], eax                    ; save return pointer
        add esp, 4
        popad

        mov eax,[ebp-4]                     ; eax = newlink
        mov ecx,[ebp-8]                     ; newLink->next = firstLink
        mov [eax+1], ecx
        mov [ebp-8], eax                    ; firstLink = newLink
        
    add edx, 2
    jmp startLoop
    endLoop:
    
    mov eax, [ebp-8]                        ; put list at eax returned value
    mov esp,ebp
    pop ebp         
    ret 

;------------------------createLink-----------------------
createLink:
    push ebp,
    mov ebp, esp
    sub esp, 8
    mov ecx,0
    mov eax,0

    pushad
    push dword 5
    call malloc
    mov [ebp-4], eax                    ; gets the pointer address
    add esp,4                           ; reseting stack after malloc
    popad
    
    pushad
    mov ebx,0
    mov bl, [ebp+9]
    push ebx
    mov bl, [ebp+8]
    push ebx
    call convertToHexa
    mov [ebp-8],eax
    add esp,8
    popad

    mov ebx, [ebp-4]
    mov eax, [ebp-8]
    mov byte[ebx],al       ; saving link's data
    mov eax, ebx           ; save returned val in eax
    mov esp,ebp
    pop ebp
    ret 

;------------------------printList-----------------------
printNumList:
    push ebp                                    ;
    mov ebp, esp    
    mov ebx, dword[ebp+8]                       ; num list to print
    mov edx,0
    pushLinks:                                  ; push number links, lsb first, to reverse prder
        mov dl, byte[ebx]
        push edx
        mov ecx, dword[ebx+1]
        mov ebx, ecx
        cmp ebx,0
    jnz pushLinks
    
        pop ebx                                     ; print msb first, no leading 0
        myPrintf [ebp+12], ebx, format_hexa1        ; [ebp+12] = FILE*
        cmp esp, ebp
        jz endPrint


    popAndPrint:                                 ; print each link, with leading 0
        pop ebx
        myPrintf stdout, ebx, format_hexa2
        cmp esp, ebp
        jnz popAndPrint
    
    endPrint:
    myPrintf stdout, new_line, format_string
    
    mov esp,ebp
    pop ebp
    ret 

;------------------------stringToHexa----------------------    
convertToHexa:
    push ebp
    mov ebp,esp
    mov eax, 0
    mov ecx, 0
    mov al, byte[ebp+8]                      ; get string's first and 2nd byte
    mov cl, byte[ebp+12]                     ; 
    
    sub al,48                                ; convert to dec value
    cmp al, 10 
    js ifAlDigit
		sub al, 7          ; add 7 if A-F
	ifAlDigit:
    
    cmp cl, 0                                ; check if second byte null
    jz ifClDigit
    
    mov bl,16
    mul bl

    sub cl,48
    cmp cl, 10
    js ifClDigit
		sub cl, 7          ; add 7 if A-F
	ifClDigit:
    add ax,cx              ; [ax] contins the num in hexa

    mov esp,ebp
    pop ebp
    ret

;------------------------pushOp----------------------    
pushOp:
    push ebp
    mov ebp,esp
    stackSizeCheck [stack_size]
    jz stackOverFlowed

    mov eax, [ebp+8]                            ; numList pointer
    mov ecx, [opEsp]
    mov [ecx],eax
    add [opEsp],dword 4
    
    jmp endPush
    stackOverFlowed:
        myPrintf stderr, sof_error, format_string

    endPush:
    mov esp,ebp
    pop ebp
    ret

;------------------------popOp----------------------    
popOp:
    push ebp
    mov ebp,esp
    stackSizeCheck 0           
    jz stackEmpty

    sub [opEsp],dword 4
    mov ecx, [opEsp]                ; eax = esp
    mov eax, [ecx]
    jmp endPop
    stackEmpty:
        myPrintf stderr, empty_error, format_string
        mov eax,0

    endPop:
    mov esp,ebp
    pop ebp
    ret

;------------------------copyNumList-----------------------
copyNumList:
    push ebp
    mov ebp,esp
    sub esp, 8

    pushad
    push dword 5
    call malloc
    add esp,4
    mov [ebp-4],eax
    popad

    mov ebx, [ebp-4]                ; returned pointer to new link
    mov ecx, dword[ebp+8]                ; pointer to the link we need to copy
    mov edx,0

    startCopyLoop:
        mov dl, byte[ecx]           ; retrieve data to copy
        mov [ebx],dl                ; copy data
        mov edx, [ecx+1]
        
        cmp edx,0                   ; check if we got to null
        jz endCopyLoop              ; than jump to end

        pushad
        push dword 5
        call malloc                 ;malloc for new link since exists
        mov [ebp-8],eax             ;retrieving the returned pointer in eax
        add esp,4
        popad

        mov eax, [ebp-8]            ; eax <- *link
        mov [ebx+1],eax             ; prev.next <- *link
        mov eax, dword[ecx+1]       ; eax<- src.next
        mov ecx,eax                 ; ecx <- src.next
        mov ebx, [ebp-8]            ; ebx <- *link
        jmp startCopyLoop
    endCopyLoop:

    mov eax, [ebp-4]
    mov esp,ebp
    pop ebp
    ret

;------------------------peekOp----------------------    
peekOp:
    push ebp
    mov ebp,esp
    stackSizeCheck 0           
    jz nothingToPeek

    sub [opEsp],dword 4 
    mov ecx, [opEsp]                ; ecx = opEsp
    mov eax,[ecx]                   ; eax = *link
    jmp endPeek
    nothingToPeek:
        myPrintf stderr, empty_error, format_string
        mov eax,0

    endPeek:
    add [opEsp],dword 4 
    mov esp,ebp
    pop ebp
    ret

;------------------------countDigits----------------------
countDigits:
    push ebp
    mov ebp, esp
    mov ecx, 0
    mov ebx, dword[ebp+8]                 ; pointer to the link we need to copy

    startCountLoop:
        mov edx, [ebx+1]                  ;
        cmp edx, 0  
        add ecx,2
        jz endCountLoop                   ; than jump to end
        mov ebx, edx
    endCountLoop:
    
    mov dl, byte[ebx]                      ; get last link data
    cmp dl, 16
    jns endCount
        sub ecx,1
    endCount:


    mov esp,ebp
    pop ebp
    ret

;------------------------sumLinks----------------------
sumLinks:                       
    push ebp
    mov ebp, esp
    sub esp,16                              ; [ebp-4] = currentSum, [ebp-8] = new allocated link,
                                            ; [ebp-12] = pointer to first link ,[ebp-16] = last pushed link
    mov eax, [ebp+8]                        ; 1st numLink
    mov ebx, [ebp+12]                       ; 2nd numLink    
    mov dword[ebp-12],0                     ; start with null
    pushfd

    startSumLoop: 
        mov edx,0  
        mov ecx,dword[eax+1]                ; ecx = num1.next
        mov dl, byte[eax]                   ; dl = num1.data
        mov byte[eax],0                     ; num1.data = 0, (in case next = null, wont add any more)
        mov [ebp-4],dl                      ; [ebp-4] = num1.data
        cmp ecx,0                           ; if num1.next != null =>
        jz isNull1
        mov eax,ecx                         ;  num1 = num1.next
        isNull1:

        mov ecx,dword[ebx+1]                 ; ecx = num2.next
        mov dl, byte[ebx]                    ; dl = num2.data
        mov byte[ebx],0                      ; num2.data = 0, (in case next = null, wont add any more)
        cmp ecx,0                            ; if num2.next != null => 
        jz isNull2
        mov ebx,ecx                          ; num2 = num2.next
        isNull2:

        popfd
        adc byte[ebp-4],dl                   ; [ebp-4] = num1.data + num2.data + cf
        pushfd

        jc carryOn
        cmp byte[ebp-4],0                    ; if [ebp-4] == 0, jump to end
        jz endSumLoop
    
        carryOn:                             ; create new link
            pushad
            push dword 5
            call malloc                      ;malloc for new link since exists
            mov [ebp-8],eax                  ;retrieving the returned pointer in eax
            add esp,4
            popad

            mov ecx, [ebp-8]                 ; ecx <- *newlink
            cmp dword[ebp-12],0
            jnz notFirstIter
                mov [ebp-12],ecx             ; [ebp-12] = firstLink
            notFirstIter:
            
            mov dl, byte[ebp-4]              ; dl <- sum
            mov byte[ecx], dl                ; ecx.data <- dl
            mov dword[ecx+1], 0              ; ecx.next <- null  
            mov edx, [ebp-16]                ; edx <- prev
            mov dword[edx+1], ecx            ; prev.next <- *newlink 
            mov [ebp-16], ecx                ; prev <- *newlink
            
            jmp startSumLoop
    endSumLoop:

    mov eax,[ebp-12]                    ; 0x56559a40, 0x56559a50
    mov esp,ebp
    pop ebp
    ret

;------------------------ShiftIfOdd----------------------
ShiftIfOdd:
    push ebp
    mov ebp,esp
    mov eax,0
    mov ecx,inputString
    startStrLenLoop:
        mov bl,[ecx]
        cmp bl,10
        jz checkIfEven
        inc eax
        inc ecx
        jmp startStrLenLoop
    checkIfEven:
        mov ebx,eax
        and ebx,1
        jz isEven
        mov ebx, inputString
        mov ecx,0
        mov edx,48
        
    startShifting:
        mov cl,dl
        mov dl,byte[ebx]
        mov [ebx], cl
        inc ebx
        cmp cl,0
        jnz startShifting
        mov byte[ebx], 0
    endShifting:
    isEven:
    mov esp,ebp
    pop ebp
    ret

;------------------------clearLeadingZeros----------------------
clearLeadingZeros:
    push ebp
    mov ebp,esp
    mov eax,0                           ; counter of leading zeros
    mov ecx,inputString                 ; the offset
    countZeros:
        mov bl,[ecx]
        cmp bl,48
        jnz finishCount
        inc eax
        inc ecx
        jmp countZeros
    finishCount:
    mov ebx,inputString
    mov edx,0
    shiftZeros:
        mov dl, [ecx]                   ; get next non zero
        mov [ebx], dl                   ; put correct spot
        cmp dl,0
        jz endClearZeros
        inc ebx
        inc ecx
        jmp shiftZeros

    endClearZeros:
    mov esp,ebp
    pop ebp
    ret

;------------------------countNumDigits----------------------
countNumDigits:
    push ebp
    mov ebp,esp
    sub esp,8                           ; [ebp-4] = digits counter, [ebp-8] = 1
    mov ebx, [ebp+8]

    pushad
    push dword 5
    call malloc
    mov [ebp-8],eax
    add esp,4
    popad

    mov eax, [ebp-8]
    mov [eax], byte 1
    mov [eax+1], dword 0

    mov ebx, [ebp+8]

    mov esp,ebp
    pop ebp
    ret