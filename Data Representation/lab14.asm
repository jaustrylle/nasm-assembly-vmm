; CS 3304
; Lab 14

%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"


SECTION .data						; initialized data

SECTION .bss						; uninitialized data
val1   		resd 		1			; reserve 1 double word
val2   		resd 		1			; reserve 1 dword
val3    	resd 		1			; reserve 1 dword

SECTION .text						; code section, actual instructions
		global _start
_start:	
	call	ReadInt
	mov		[val1], eax				; val1
		
	call	ReadInt
	mov		[val2], eax				; val2
			
	call	ReadInt
	mov		[val3], eax
	
	; display memory
	mov		esi, val1				; place address of storage area in esi
	mov		ecx, 3					; place # of units to display in ecx
	mov		ebx, 4					; unit type (1=byte, 2=word, 4=dword)
	call	DumpMem					; call external routine DumpMem
	
	; calculations
	mov     eax, [val3]       		; eax = 3
	call    DumpRegs

	mov     eax, [val1]        		; eax = 1
	call    DumpRegs

	sub     eax, [val2]       		; 1 - 2 = -1
	call    DumpRegs

	sub     eax, [val2]        		; -1 - 2 = -3
	call    DumpRegs

	sub     eax, [val3]        		; -3 - 3 = -6
	call    DumpRegs
	
	; show the mem after calculations
	mov		esi, val1
	mov		ecx, 3
	mov		ebx, 4
	call	DumpMem

		Exit	{0}