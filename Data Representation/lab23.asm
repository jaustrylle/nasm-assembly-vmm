; CS 3304
; Lab 23

[list -]
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"
[list +]

; just turn in lab23.asm, no main func

;---------------------------------------------------------------------
global GenerateRandomString:function
; HLL prototype: void GenerateRandomString(char *string, int n);
; This function generates a string of length n, containing all
; capital letters. The random letters are placed in the string 
; beginning at the lowest offset.
; Receives: ESI = starting offset of string
;           ECX = length of string
; Returns:  nothing
;---------------------------------------------------------------------
GenerateRandomString:
SECTION .text
		pushad
			
gen_loop:
		cmp			ecx, 0
		je			done
		
		mov			eax, 26
		call		RandomRange			; generate a random num in range 0-25
				
		add			al, 65				; range is now 65-90
		mov			[esi], al		; to where esi point, a char
		
		inc			esi
		dec			ecx
		jmp			gen_loop
		
done:
		popad
		ret

SECTION .text
		
SECTION .data

SECTION .bss
