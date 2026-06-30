; CS 3304
; Lab 22

[list -]
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"
[list +]

;---------------------------------------------------------------------
global  ReadFirstTwo:function
; HLL prototype: void ReadFirstTwo(int *array);
; Accepts two 32-bit signed integers from standard input and uses
; these two values to initialize the first two elements of an array.
; These two values represent the first two terms in a Fibonacci
; sequence.
; Receives: ESI = starting offset of array
; Returns:  nothing
;---------------------------------------------------------------------
ReadFirstTwo:			; read 2 int from stdin
SECTION .text
		pushad						; saves the 32-bit GP reg to sys stack
		
		call	ReadInt				; read a 32-bit signed int from stdin
		mov		[esi], eax			; store at the 1st elem of array
		
		call	ReadInt				; read a 32-bit signed int from stdin
		mov		[esi + 4], eax		; store at 2nd elem of array
		
		popad						; restore 32-bit GP reg from sys stack
		ret
;---------------------------------------------------------------------
global  FillArray:function    
; HLL prototype: void FillArray(int *array, int n);
; Given the first two elements of a Fibonacci sequence, fill the
; remaining elements of the array with the next terms in the 
; sequence.
; Receives: ESI = starting offset of array
;           ECX = # of elements in array
; Returns:  nothing
;---------------------------------------------------------------------
FillArray:				; compute fib using esi for array, ecx for size
SECTION .text
		pushad			; saves the 32-bit GP reg to sys stack
		
		; if size <= 2, do nothing
		cmp			ecx, 2
		jle			done_fill
		
		mov			edi, 2
		
	fill_loop:
		cmp			edi, ecx
		jge			done_fill
		
		; fib[i] = fib[i-1] + fib[i-2]
		mov			eax, [esi + edi*4 - 4]
		add			eax, [esi + edi*4 - 8]
		mov			[esi + edi*4], eax
		
		inc			edi
		jmp			fill_loop
	
	done_fill:
		popad			; restore 32-bit GP reg from sys stack
		ret
;---------------------------------------------------------------------
global  DisplayArray:function
; HLL prototype: void DisplayArray(int *array, int n);
; Displays the elements of the n-element array to standard output,
; one element per line. Each element is preceded by a descriptive
; literal.
; Receives: ESI = starting offset of array
;           ECX = # of elements in array
; Returns:  nothing
;---------------------
DisplayArray:			; display array as esi for array, ecx for size
SECTION .text
		pushad							; saves the 32-bit GP reg to sys stack
		mov		dword[index], 0		;initialize index to 0
		
printLoop:						; prints format 'Fib[#] = +#'
		mov		edx, lit1
		call	WriteString
		
		mov		eax, [index]	; move index to A reg
		call	WriteDec		; write a 32-bit unsigned in to stdout
		
		mov		edx, lit2
		call	WriteString
		
		mov		eax, [esi]		; move to the A reg what esi points to
		call	WriteInt		; write a 32-bit signed int to stdout
		
		mov		al, `\n`		; use left quote above tab
		call	WriteChar
		
		inc		dword [index]	; add 1 to index
		add		esi, 4			; calc address of next elem of array
		
		loop	printLoop		; prints format 'Fib[' 47 times
		
		popad					; restore 32-bit GP reg from sys stack
		ret
		
SECTION .data
lit1	db		'Fib[', 0		; terminate with 0/null
lit2	db		'] = ', 0

SECTION .bss
index	resd	1