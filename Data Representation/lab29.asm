; CS 3304
; Lab 29

[list -]
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"
[list +]

%if 0
1. Sieve of Eratosthenes for prime numbers
	- Finds all prime num <= n, all primes 2-n basically
	- 2 is prime num, but mult of 2 NOT (cross out)
	- Then 3 is prime, but not its mults
	- Then 5, but not its mults
%endif

;---------------------------------------------------------------------
global  FillArray       
; HLL prototype: void FillArray(byte *array, int n);
; Sets array[0] = array[1] = 0 and array[i] = 1, i = 2, 3, ..., (n-1)
; Receives: ESI = starting offset of array
;           ECX = # of elements in array
; Returns:  nothing
;---------------------------------------------------------------------
FillArray:

SECTION .text
		pushad						; saves the 32-bit GP reg to sys stack
		
		mov			byte[esi], 0	; array[0] = 0
		mov			byte[esi+1], 0	; array[1] = 0
		mov			edi, 2			; i = 2
		
	fillLoop:
		cmp			edi, ecx
		jge			endLoop
		
		mov			byte[esi+edi], 1
		inc			edi
		jmp			fillLoop
		
	endLoop:
		popad						; restore 32-bit GP reg from sys stack
		ret
		
;---------------------------------------------------------------------
global  EliminateMultiples
; HLL prototype: void EliminateNultiples(byte *array, int n, int k);
; Implements the following loop:
;      for (i = k + k; i < n; i += k)
;        array[i] = 0            
; For example, if k = 2, sets array[4], array[6], array[8], ... to 0
; Receives: ESI = starting offset of array
;           ECX = # of elements in array
;           EDX = value of k
; Returns:  nothing
;---------------------------------------------------------------------
EliminateMultiples:

SECTION .text
		pushad						; saves the 32-bit GP reg to sys stack		
		
		; esi -> array, ecx -> n, edx -> k
		
		mov			eax, edx		; i = k + k
		add			eax, edx		; eax = 2k
	
	whileLoop:
		cmp			eax, ecx
		jge			done
		
		mov			byte[esi+eax], 0
		
		add			eax, edx		; i += k
		jmp			whileLoop
		
	done:
		popad						; restore 32-bit GP reg from sys stack
		ret
		
;---------------------------------------------------------------------
global  DisplayArray
; HLL prototype: void DisplayArray(byte *array, int n);
; Implements the following loop:
;      for (i = 2; i < n; ++i)
;        if array[i] == 1 then print i
; Receives: ESI = starting offset of array
;           ECX = # of elements in array
; Returns:  nothing
;---------------------------------------------------------------------
DisplayArray:

SECTION .text
		pushad						; saves the 32-bit GP reg to sys stack		
		
		; esi -> array, ecx -> n
		
		mov			edi, 2			; i = 2
		
	printLoop:
		cmp			edi, ecx
		jge			printed
		
		cmp			byte[esi+edi], 1
		jne			skip			; skip non-prime #saves
		
		mov			eax, edi		; # to print
		call		WriteDec		; prints dec
		mov			al, 0Ah			; avoid Windows carriage
		call		WriteChar		; prints char
		
	skip:
		inc			edi
		jmp			printLoop
		
	printed:
		popad						; restore 32-bit GP reg from sys stack
		ret
		
;---------------------------------------------------------------------
; ../irvine_test.sh lab29 01.dat > my.out
;						  02.dat
;						  03.dat
;						  04.dat
;
; cat -A my.out
; cat -A 01.out
;---------------------------------------------------------------------