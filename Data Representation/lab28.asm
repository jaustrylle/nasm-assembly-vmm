; CS 3304
; Lab 28

[list -]
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"
[list +]

%if 0
1. Receive signed 32-bit int, display internal octal representation
2. Use bt instruction to access the bits
3. No shift/div instruction allowed
%endif

;---------------------------------------------------------------------
global  PrintOctal
; HLL prototype: void PrintOctal(int n);
; Prints the internal octal representation of n
; Receives: EAX = signed 32-bit integer
; Returns:  nothing
;---------------------------------------------------------------------

PrintOctal:			; PRINT OCTAL FUNCTION

SECTION .text
		pushad						; saves the 32-bit GP reg to sys stack
		
		mov			ebx, eax		; save orig input
		mov			edx, 0			; 1st group
		
		; bit #31 of weight 2
		bt			ebx, 31
		jc			first_bit
		jmp			second_bit
		
	first_bit:
		add			edx, 2
		
	second_bit:
		; bit #30 of weight 1
		bt			ebx, 30
		jc			set_2nd_bit
		jmp			print_1st
		
	set_2nd_bit:
		add			edx, 1
		
	print_1st:
		add			dl, 00110000b
		mov			al, dl
		call		WriteChar
		
		; group bits into 3's
		mov			ecx, 29				; start loop at bit #29
		
whileLoop:
		mov			edx, 0			; while (ecx >= 0) do
		
		; bit #1 of weight 4
		bt			ebx, ecx
		jc			set_bit1
		jmp			bit2
		
	set_bit1:
		add			edx, 4
		
	bit2:
		; bit #2 of weight 2
		mov			esi, ecx
		dec			esi				; esi = ecx - 1
		bt			ebx, esi
		jc			set_bit2
		jmp			bit3			; temp reg to avoid invalid syntax
		
	set_bit2:
		add			edx, 2
		
	bit3:
		; bit #3 of weight 1
		mov			esi, ecx
		sub			esi, 2			; esi = ecx - 2
		bt			ebx, esi
		jc			set_bit3
		jmp			printLoop
		
	set_bit3:
		add			edx, 1
		
	printLoop:
		add			dl, 00110000b	; convert ASCII
		mov			al, dl
		call		WriteChar
		
		sub			ecx, 3			; move to next group
		cmp			ecx, -1			; stop after last group
		jg			whileLoop
		
		popad						; restore 32-bit GP reg from sys stack
		ret
		
; to run: ../irvine_test.sh lab28 01.dat > my.out
;---------------------------------------------------------------------