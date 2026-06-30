; CS 3304
; Lab 27

[list -]
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"
[list +]

%if 0
1. Receive signed 32-bit int
2. Display that int's quaternary representation
3. Use bt instruction, no shift/div allowed
%endif

;---------------------------------------------------------------------
global  PrintQuaternary:function
; HLL prototype: void PrintQuaternary(int n);
; Prints the internal quaternary representation of n
; Receives: EAX = signed 32-bit integer
; Returns:  nothing

PrintQuaternary:

SECTION .text
    pushad

    mov esi, eax        ; copy eax 1x, use stable reg after
    mov ecx, 31

whileLoop:
    cmp ecx, 1
    jl endWhile

    xor ebx, ebx

    ; ---- high bit ----
    bt esi, ecx
    setc bh

    ; ---- low bit ----
    mov edx, ecx
    dec edx
    bt esi, edx
    setc bl

    ; ---- combine the 2 bits ----
    add bh, bh          ; high num * 2
    add bl, bh          ; num + low

    ; ---- print the result ----
    add bl, '0'
    mov al, bl
    call WriteChar      ; destroys eax if esi not used

    sub ecx, 2
    jmp whileLoop

endWhile:
    popad
    ret