; CS 3304
; Lab 24

[list -]
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"
[list +]

SECTION .data
idLine	db		`Test Testman - CS 3304 - Lab 24\n\n`, 0
lit1	db		` is negative\n`, 0
lit2	db		` is not negative\n`, 0

SECTION .bss
H	resd		1			; reserve a dword

SECTION .text
	global	_start
	
; to run: ../irvine_test.sh lab24 01.dat
	
_start:
	
	mov		edx, idLine
	call	WriteString
	
	call	ReadInt			; read a 32-bit signed int from stdin
	mov		[H], eax		; and store at H
	
	mov		ecx, [H]		; set loop control
	
readloop:
	call	ReadInt			; read a 32-bit signed int from stdin
	call	WriteInt		; and echo to stdout
	
	add		eax, 0			; get flag, option1 = add 0 to num
	js		isNegative		; tests sign flag if neg by jmp if neg
	mov		edx, lit2
	jmp		writeMsg		; unconditional jump
	
isNegative:
	mov		edx, lit1
	
writeMsg:
	call	WriteString
	
	loop	readloop

	Exit {0}