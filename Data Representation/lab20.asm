; CS 3304
; Lab 20

%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"
[list +]

; shift el of int array forward 1 pos
; val at end of array wraps around to 1st position
; length - 1 loop exec, no copy el to other array

SECTION .data						; initialized data
	myArray 	times 		40 dd 0
	length 		equ 		($ - myArray) / 4

SECTION .bss						; uninitialized data

SECTION .text
	global	_start
	
_start:

	; display initial array of all 0s
	mov		esi, myArray	; place address in esi
	mov		ecx, length		; # of units
	mov		ebx, 4			; unit type (1=byte, 2=word, 4=dword)
	call	DumpMem			; call external routine to dump mem
	
	; read 40 int values into array
	mov		esi, myArray	; initialize array using input
	mov		ecx, length
	
read_loop:
	call	ReadInt			; read 32-bit signed int, stdin; print after initializing
	mov		[esi], eax		; store int
	add		esi, 4			; 4 because of array of dw, inc only adds 1
	loop	read_loop
	
	; display initialized array
	mov		esi, myArray
	mov		ecx, length
	mov		ebx, 4
	call	DumpMem
	
	; shift array forward, all elems by 1
	mov		esi, myArray	; reverse array in place
	mov		ecx, length		; save last el
	dec		ecx
	mov		eax, [esi + (ecx * 4)]	; last el saved
	
	; shift elems right
	mov		ecx, length
	dec		ecx				; start from (last_index - 1)
	
shift_loop:		; must shift in place
	mov		edx, [esi + (ecx - 1) * 4]
	mov		[esi + ecx * 4], edx
	loop	shift_loop
	
	; put the last el at the front
	mov		[esi], eax
	
	; display the shifted array
	mov		esi, myArray
	mov		ecx, length
	mov		ebx, 4
	call	DumpMem
	
	; exit routine
	mov		eax, 1
	int		0x80
	
	; registers eax, ebx for temps
	; to run -> ../irvine_test.sh lab20 01.dat
	
		Exit {0}