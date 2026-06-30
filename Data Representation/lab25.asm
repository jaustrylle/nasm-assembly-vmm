; CS 3304
; Lab 25

[list -]
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"
[list +]

%if 0
1. Reads header H
2. Reads H additional signed 32-bit int
3. No using shift/division instructions
4. Display int to stdout + descr lit msg
%endif

SECTION .data
idLine      db  "Test Testman - CS 3304 - Lab 25", 10, 10, 0
msgDiv      db  " is divisible by 4", 10, 0
msgNotDiv   db  " is not divisible by 4", 10, 0

SECTION .bss
H       resd    1
num     resd    1

SECTION .text
    global _start

_start:

    ; print header
    mov edx, idLine
    call WriteString

    ; read H
    call ReadInt
    mov [H], eax

    mov ecx, [H]

read_loop:
    cmp ecx, 0
    je done

    ; read number
    call ReadInt
    mov [num], eax

    ; check divisibility
    mov eax, [num]
    and eax, 3
    mov ebx, eax        ; save result

    ; print number checking
    mov eax, [num]
    call WriteInt

    cmp ebx, 0
    je divisible

not_divisible:
    mov edx, msgNotDiv
    call WriteString
    jmp continue

divisible:
    mov edx, msgDiv
    call WriteString

continue:
    dec ecx
    jmp read_loop

done:
    Exit {0}