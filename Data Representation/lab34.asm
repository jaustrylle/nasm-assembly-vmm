; CS 3304
; Lab 34

%if 0
1. multiply 2 unsigned 32-bit int 
2. use only shift and addition
3. product ret in eax reg (could be as large as 64 bits)
4. assume product never larger than 32 bits
%endif

[list -]
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"
[list +]


;---------------------------------------------------------------------
global  BitwiseMultiply:function
; HLL prototype: 
;   uint BitwiseMultiply(uint *multiplicand, uint *multiplier);
; Receives pointers to two unsigned 32-bit integers.
; Determines the product of the two integers using the bitwise
; multiplication method. Returns the product in the EAX register.
; Receives: EAX = address of multiplicand
;           EBX = address of multiplier
; Returns:  EAX = product of multiplicand and multiplier
;---------------------------------------------------------------------
BitwiseMultiply:
	SECTION .text
		pushad
		
		; dereference a & b reg to access contents
		mov		esi, eax	; esi is multiplicand
		mov		edi, ebx	; edi is multiplier
		
		mov		eax, [esi]	; eax is multiplicand
		mov		ebx, [edi]	; ebx is multiplier
		
		xor		edx, edx	; edx is result (0)
		
		; eax is address of multiplicand, ebx is address of multiplier
		; output eax which will hold the product
		
		; if bit is 1, add multiplicand to result
		; shift multiplicand left, shift multiplier right
		
	multiply_loop:
		cmp		ebx, 0
		je		done
		
		test	ebx, 1		; check LSB of multiplier
		jz		skip_add
		
		add		edx, eax	; result += multiplicand
	
	skip_add:
		shl		eax, 1		; multiplicand <<= 1
		shr		ebx, 1		; multiplier >>= 1
		jmp		multiply_loop
		
	done:
		mov		[esp+28], edx	; overwrite saved eax from stk
		
		popad
		ret
;---------------------------------------------------------------------
