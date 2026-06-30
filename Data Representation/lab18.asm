; CS 3304
; Lab 18

%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"

%ifdef LAB_17_DIRECTIONS
esi = start of src, edi = start of tgt, ecx = # bytes in length, use a loop
%endif

SECTION .data
source db "This is the source string",0
length equ $ - source
target times length db '#'

SECTION .bss

SECTION .text
		global _start
_start:
	; display memory of src
	mov		esi, source		; esi = starting offset
	mov		ecx, length		; ecx = # of units
	mov		ebx, 1			; ebx = unit size (1=byte, 2=word, 4=dword)
	call	DumpMem
	
	; display memory of tgt
	mov		esi, target
	mov		ecx, length
	mov		ebx, 1
	call	DumpMem
	
	; use loop to reverse-copy string
	mov		esi, source		; src
	mov		edi, target		; tgt
	add		edi, length
	dec		edi				; last tgt byte
	
	mov		ecx, length
	
	reverse_loop:
	mov		al, [esi]		; read src
	mov		[edi], al		; backward write to tgt
	
	inc		esi				; forward in src
	dec		edi				; backward in tgt
	
	loop reverse_loop
	
	; display mem again after copy, src
	mov		esi, source
	mov		ecx, length
	mov		ebx, 1
	call	DumpMem
	
	; display mem again after copy, tgt
	mov		esi, target
	mov		ecx, length
	mov		ebx, 1
	call	DumpMem

		Exit {0}