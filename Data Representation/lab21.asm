; CS 3304
; Lab 21

%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"
[list +]

; read val for N from stdin as uninitialized dw
; use loop to acc N dw from stdin, push each to sys stack
; use 2nd loop to dis orig invals in rev order
; place orig val from stack 1 at a time in eax registers
; dumpreg

SECTION .data						; initialized data

SECTION .bss						; uninitialized data
N	resd	1		; reserve 1 dword

SECTION .text						; code section
	global	_start
_start:

	; stacks are LIFO, pop will print vals rev order auto
	
	call	ReadInt			; read a 32-bit signed int from stdin
	mov		[N], eax		; and store at N
	
	mov		ecx, eax		; counter for loop w/input	
	
read_loop:
	cmp		ecx, 0
	je		done_read
	
	call	ReadInt			; read 32-bit signed int, stdin
	push	eax				; push int to system stack
	
	loop	read_loop
	
done_read:
	mov		ecx, [N]		; reset counter for output
	
	mov		ebx, 1			; initialize ebx once
	
output_loop:
	cmp		ecx, 0
	je		done
	
	pop		eax				; get val from stack
	call	DumpRegs
	
	loop	output_loop
	
done:
	; to run -> ../irvine_test.sh lab21 01.dat
	
	; ebx, edx, esi, esp, eip, efl depend on:
	; mem layout, os state, stack loc, runtime env, & mach/sess
		Exit {0}