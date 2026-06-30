; CS 3304
; Lab 26

[list -]
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"
[list +]

%if 0
1. Use bt instruction to access the bits, no shift/division allowed
2. Receive signed 32-bit int
3. Display that int's internal binary rep
%endif

;---------------------------------------------------------------------
global  PrintBinary:function
; HLL prototype: void PrintBinary(int n);
; Prints the internal binary representation of n
; Receives: EAX = signed 32-bit integer
; Returns:  nothing

PrintBinary:			; PRINT BINARY FUNCTION

SECTION .text
		pushad						; saves the 32-bit GP reg to sys stack
		
		; 32-bit int has 31 bits til 0
		; bt eax, index tests a bit
		; result of bit test stored in CF
		
		mov			ebx, eax		; keep orig num
		mov			ecx, 31			; loop from 31 down to 0 using MSB
		
whileLoop:
		
		; step 1 - test loop control variable
		bt			ebx, ecx			; while (ecx >= 0) do
		
		; jb means unsigned - when it gets to 0, you can't subtract 1
		; it thus ends up in an infinite loop
		; use jl for signed instead

		jc			itsOne			; if CF(1), then bit=1
		
		; ch. 6 - BT instruction (Bit Test), using r32
		; WriteChar routine expects N to be in A reg
		
		mov			al, 00110000b			; ASCII '0'
		call		WriteChar
		jmp			getNextBit
		
itsOne:
		mov			al, 00110001b			; ASCII '1'
		call		WriteChar
		
getNextBit:

		; we now have a 0 or 1 based on the A reg
		; add 48 to convert a bin dec byte into equiv ASCII dec dig - Ch. 6
		; OR instruction to set bits 4 & 5
		
		dec			ecx
		cmp			ecx, -1			; cleanup
		
		jne			whileLoop		; loop only until ecx < 0
		
		; or			al, 00110000b	; add 48 to al, because 0 char is coded 48
		; call		WriteChar		; now a printable char, so PRINT
		; pop			eax				; restore eax from sys stack
		; dec			ecx				
		
		popad						; restore 32-bit GP reg from sys stack
		ret

;---------------------------------------------------------------------