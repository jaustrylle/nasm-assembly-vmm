; CS 3304
; Lab 38

; ------------------------------------------------------------------------------------ ;
%if 0

Recursive printing of:
1. Binary (base 2)
2. Octal (base 8)
3. Hexadecimal (base 16)

- don't use any "global" vars
- use the system stack for any local vars

Build:
nasm -f elf32 -o lab38.o lab38.asm
gcc -m32 -o lab38 lab38main.o lab38.o /usr/local/3304/src/Along32.o -lm

%endif
; ------------------------------------------------------------------------------------ ;

[list -]
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"
[list +]

extern putchar		; prints 1 char

; move L->R to print
; 1. divide # by base
; 2. recursively print quotient first
; 3. print remainder dig last

; neg nums will always get the right # of dig, pos # easy

; ------------------------------------------------------------------------------------ ;

; PrintBinary is a recursive assembly-language function that writes
; the binary (base 2) representation of n to stdout
global PrintBinary:function

PrintBinary:
	SECTION .text		; only text
		push	ebp
		mov		ebp, esp
		
		mov		eax, [ebp+8]	; n
		
		mov		edx, eax
		shr		edx, 1			; n/2 logical
		
		cmp		edx, 0
		je		.print_digit
		
		push	edx
		call	PrintBinary
		add		esp, 4
		
		.print_digit:
			mov		eax, [ebp+8]	; orig n
			
			and		eax, 1		; n%2
			add		eax, '0'
			
			push	eax
			call	putchar
			add		esp, 4
			
			pop		ebp
			ret

; ------------------------------------------------------------------------------------ ;

; PrintOctal is a recursive assembly-language function that writes
; the octal (base 8) representation of n to stdout
global PrintOctal:function

PrintOctal:
	SECTION .text		; only text
		push	ebp
		mov		ebp, esp
		
		mov		eax, [ebp+8]		; n
		
		mov		edx, eax
		shr		edx, 3				; n/8 logical
		
		cmp		edx, 0
		je		.print_digit
		
		push	edx
		call	PrintOctal
		add		esp, 4
		
		.print_digit:
			mov		eax, [ebp+8]	; orig n
			
			and		eax, 7		; n%8
			add		eax, '0'
			
			push	eax
			call	putchar
			add		esp, 4
			
			pop		ebp
			ret

; ------------------------------------------------------------------------------------ ;

; PrintHexadecimal is a recursive assembly-language function that writes
; the hexadecimal (base 16) representation of n to stdout
global PrintHexadecimal:function

PrintHexadecimal:
	SECTION .text		; only text
		push	ebp
		mov		ebp, esp
		
		mov		eax, [ebp+8]		; n
		
		mov		edx, eax
		shr		edx, 4				; n/16 logical
		
		cmp		edx, 0
		je		.print_digit
		
		push	edx
		call	PrintHexadecimal
		add		esp, 4
		
		.print_digit:
			mov		eax, [ebp+8]	; orig n
			
			and		eax, 15			; n%16
			cmp		eax, 9
			jle		.num
			
			add		eax, 55			; 10=A, B...
			jmp		.out
			
		.num:
			add		eax, '0'
			
		.out:
			push	eax
			call	putchar
			add		esp, 4
			
			pop		ebp
			ret
	
; ------------------------------------------------------------------------------------ ;
; to run:
; gcc -m32 -c lab38main.c
; nasm -f elf32 -o lab38.o lab38.asm -I/usr/local/3304/include/
; gcc -m32 -o lab38 lab38main.o lab38.o /usr/local/3304/src/Along32.o -lm
; ./lab38 < 01.dat

; c file in assembly?

; ./lab38 < 01.dat > my.out
; diff 01.out my.out
