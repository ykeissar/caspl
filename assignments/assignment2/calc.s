;Macro start

%define MAX_INPUT_LENGTH 80

%macro getUserInput 0
    myPrintf stdout,default_msg
    pushad
    pushfd
    push dword [stdin]                ; 3rd arg - FILE pointer
    push MAX_INPUT_LENGTH             ; 2nd arg - string length
    push dword inputString           ; 1st arg - maybe need to clean inputString
    call fgets
    myFlush stdin
    add esp, 12
    popfd
    popad
%endmacro

%macro myPrintf 2+
    pushad
    pushfd
    push dword %2             ; 3rd arg, string pointer
    push dword format_string  ; 2nd arg, format string
    call printf     
    myFlush %1
    add esp, 8
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
    push dword [%1]                     ; 1st arg, FILE pointer
    call fflush
    add esp, 4
    popfd
    popad
%endmacro    

;Macro end

section .rodata
    format_string: db "%s", 0	   ; format string
    default_msg: db "calc: ", 0    ;

section .bss
    inputString: resb MAX_INPUT_LENGTH        ; max input size is 80 chars
    
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

main:
    startFunc 0         ;

    getUserInput        ;
    endFunc 0           ;
