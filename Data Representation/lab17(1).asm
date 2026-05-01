%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

%ifdef LAB_17_DIRECTIONS
1. Use a loop to calc the first 12 terms of a Fibonacci num seq
	0, 1, 1, 2, 3, 5, 8, 13, 21, ...
%endif

SECTION .data

SECTION .bss
term1		resd		1
term2		resd		1
next		resd		1

SECTION .text
		global _start
_start:
	call	ReadInt				; read 1st input
	mov		[term1], eax
	
	call	ReadInt				; read 2nd input
	mov		[term2], eax
	
	; dump both terms
	mov		eax, [term1]
	call	DumpRegs
	
	mov		eax, [term2]
	call	DumpRegs
	
	; use loop to gen next terms in seq
	mov		ecx, 10		; counter index
	
	fibonacci_loop:
	
	mov		eax, [term1]
	add		eax, [term2]
	mov		[next], eax
	
	; prev 2 terms get summed to make next 2 terms
	mov		ebx, [term2]
	mov		[term1], ebx
	mov		[term2], eax
	call	DumpRegs
	
	loop	fibonacci_loop

		Exit	{0}