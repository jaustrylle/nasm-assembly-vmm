; CS 3304
; Lab 10

; track contents of A register, 5 dumps
; 1000₁₆ + 100₈ + 1000₂ + 1000₁₀
; registers display in hex, convert all numbers to hex

%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

SECTION .data						; initialized data
; use only immediate operands

SECTION .bss						; uninitialized data
; use only immediate operands, also it's addition

SECTION .text						; code section, actual instructions
		global _start
_start:
		mov				eax,0			; initialize the eax register to 0
		call			DumpRegs
		
		add				eax,1000h		; 1000 b 16
		call			DumpRegs
		
		add				eax,100q		; 100 base 8
		call 			DumpRegs
		
		add				eax,1000b		; 1000 base 2
		call			DumpRegs
		
		add				eax,1000		; 1000 base 10
		call			DumpRegs
		
		Exit	{0}