; CS 3304
; Lab 35

; ------------------------------------------------------------------------------------ ;

%if 0
Hamming Character Code - single-bit errors can be detected/corrected
* Each 7-bit ASCII char embedded in 11-bit code (Hamming char)
* Bit pos of char numbered from MSB to LSB

pos: 1	2	3	4	5	6	7	8	9	10	11
bit: p	p	d	p	d	d	d	p	d	d	d
%endif

; ------------------------------------------------------------------------------------ ;

%if 0
Bits 1, 2, 4, 8 are parity bits (powers of 2)

bit1 = (bit3 + bit5 + bit7 + bit9 + bit11) % 2
bit2 = (bit3 + bit6 + bit7 + bit10 + bit11) % 2
bit4 = (bit5 + bit6 + bit7) % 2
bit8 = (bit9 + bit10 + bit11) % 2

Bits 3, 5, 6, 7, 9, 10, 11 are data bits (MSB-LSB)
%endif

; ------------------------------------------------------------------------------------ ;

[list -]
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"
[list +]

; ASCII char 'M' has 7-bit code 1001101 -> 01110010101

; our num scheme = 11 - Hamming

; text encoded as seq of Hamming char transmitted through channel
; channel can introduce at most 1 bit err per Hamming char

; determine loc of err & correct as:
; 01110010101 is 'M', received as 01110010001
; loc of errant bit computed as sum of pos of parity bits disagreeing with recomp vals
; offending bit complemented to determine correct Hamming char (01110010001)
; comp bit1, bit2, bit4, bit8 (1+8=9), bit9 = offending bit
; works with all Hamming chars with 1 bit error
; no bit error -> no discrepancies between parity bits and recomputed vals

; complement bit in that spot to fix err

; message recorded as input text file, seq of signed 32-bit int (1 per line)
; each int rep single Hamming char with at most 1 bit err
; input preceded by unsigned 32-bit int H, header rep # chars in msg

; decode msg, correcting for single-bit errs, input read from stdin w/redirection
; decoded msg written to stdout as chars, not ints

SECTION .data						; initialized data
msg 	db 			"Test Testman - CS 3304 - Lab 35", 0
hamming db 			"Hamming", 0
newline db 			10, 0

SECTION .bss						; uninitialized data

SECTION .text						; actual instructions
		global _start
_start:
	; header
	mov		edx, msg
	call	WriteString
	
	mov 	edx, newline
	call 	WriteString

	mov 	edx, newline
	call 	WriteString
	
	; read numbers loop control
	call	ReadDec
	mov		ebp, eax	; ebp as safe ctr
	test	ebp, ebp	; ctr
	jz		finish
	
	ReadLoop:
		push	ebx
		
		call	ReadInt		; read encoded int
		and		eax, 07FFh
		mov		ebx, eax
		xor		esi, esi
		
		jmp		chk_p1		; 1st check
		
	chk_p1:
		; --- 1st Parity Set (1,3,5,7,9,11) ---	
		xor		dl, dl		; clear accumulator
		
		; --- Parity 1 ---
		mov		eax, ebx
		shr		eax, 10		; bit 1
		and		al, 1
		xor		dl, al
		
		; --- Parity 3 ---		
		mov		eax, ebx
		shr		eax, 8		; bit 3
		and		al, 1
		xor		dl, al
		
		; --- Parity 5 ---
		mov		eax, ebx
		shr		eax, 6		; bit 5
		and		al, 1
		xor		dl, al
		
		; --- Parity 7 ---
		mov		eax, ebx
		shr		eax, 4		; bit 7
		and		al, 1
		xor		dl, al
		
		; --- Parity 9 ---			
		mov		eax, ebx
		shr		eax, 2		; bit 9
		and		al, 1
		xor		dl, al
		
		; --- Parity 11 ---
		mov		eax, ebx	; reload ebx into eax
		and		al, 1		; isolate bit 11
		xor		dl, al		; bit into dl
		
		; test bit
		test	dl, 1		; check if bit sum is odd
		jz		chk_p2
		add		esi, 1		; add weight 1 to syndrome
		
	chk_p2:
		; --- 2nd Parity Set (2,3,6,7,10,11) ---
		xor		dl, dl		; clear accumulator for set 2
		
		; --- Parity 2 ---
		mov		eax, ebx
		shr		eax, 9		; bit 2
		and		al, 1		; isolate the bit
		xor		dl, al		; store result in dl
		
		; --- Parity 3 ---
		mov		eax, ebx
		shr		eax, 8		; bit 3
		and		al, 1
		xor		dl, al
		
		; --- Parity 6 ---
		mov		eax, ebx
		shr		eax, 5		; bit 6
		and		al, 1
		xor		dl, al

		; --- Parity 7 ---
		mov		eax, ebx
		shr		eax, 4		; bit 7
		and		al, 1
		xor		dl, al

		; --- Parity 10 ---
		mov		eax, ebx
		shr		eax, 1		; bit 10
		and		al, 1
		xor		dl, al

		; --- Parity 11 ---
		mov		eax, ebx	; bit 11
		and		al, 1
		xor		dl, al
		
		; test bit
		test	dl, 1
		jz		chk_p4
		add		esi, 2
		
	chk_p4:
		; --- 3rd Parity Set (4,5,6,7) ---
		xor		dl, dl		; clear accumulator for set 3
		
		; --- Parity 4 ---
		mov		eax, ebx
		shr		eax, 7		; bit 4
		and		al, 1
		xor		dl, al
		
		; --- Parity 5 ---
		mov		eax, ebx
		shr		eax, 6		; bit 5
		and		al, 1
		xor		dl, al
		
		; --- Parity 6 ---
		mov		eax, ebx
		shr		eax, 5		; bit 6
		and		al, 1
		xor		dl, al
		
		; --- Parity 7 ---
		mov		eax, ebx
		shr		eax, 4		; bit 7
		and		al, 1
		xor		dl, al
		
		; test
		test	dl, 1		; check if bit sum is odd
		jz		chk_p8
		add		esi, 4		; add weight 4 to syndrome
		
	chk_p8:
		; --- 4th Parity Set (8,9,10,11) ---
		xor		dl, dl		; clear accumulator for set 4
		
		; --- Parity 8 ---
		mov		eax, ebx
		shr		eax, 3		; bit 8		
		and		al, 1
		xor		dl, al
		
		; --- Parity 9 ---
		mov		eax, ebx
		shr		eax, 2		; bit 9
		and		al, 1
		xor		dl, al
		
		; --- Parity 10 ---
		mov		eax, ebx
		shr		eax, 1		; bit 10
		and		al, 1
		xor		dl, al
		
		; --- Parity 11 ---
		mov		eax, ebx
		shr		eax, 0		; bit 11
		and		al, 1
		xor		dl, al
		
		; test bit
		test	dl, 1		; check if bit sum is odd
		jz		fixbit
		add		esi, 8		; add weight 8 to syndrome

	fixbit:
		; esi contains binary sum of all failing parity checks
		test	esi, esi
		jz		decode		; no err if syndrome 0
		
		; correct bit -> pos 1 is bit 10, pos 11 is bit 0
		mov		eax, 11
		sub		eax, esi	; eax = shift amount, err 2 bits from R
		
		xor		ecx, ecx	; clear ecx
		mov		cl, al		; move shift count to cl
		
		mov		edx, 1
		shl		edx, cl
		xor		ebx, edx	; flip the bit in ebx
		
	; extract data bits (3, 5, 6, 7, 9, 10, 11) into eax
	decode:
		xor eax, eax

		; bit3
		mov edx, ebx
		shr edx, 8
		and edx, 1
		shl edx, 6
		or eax, edx

		; bit5
		mov edx, ebx
		shr edx, 6
		and edx, 1
		shl edx, 5
		or eax, edx

		; bit6
		mov edx, ebx
		shr edx, 5
		and edx, 1
		shl edx, 4
		or eax, edx

		; bit7
		mov edx, ebx
		shr edx, 4
		and edx, 1
		shl edx, 3
		or eax, edx

		; bit9
		mov edx, ebx
		shr edx, 2
		and edx, 1
		shl edx, 2
		or eax, edx

		; bit10
		mov edx, ebx
		shr edx, 1
		and edx, 1
		shl edx, 1
		or eax, edx

		; bit11
		mov edx, ebx
		and edx, 1
		or eax, edx

	; writing
	call	WriteChar
	
	pop		ebx
	
	dec		ebp				; dec safe loop ctr
	jnz		ReadLoop
	jmp		finish
	
	finish:
		Exit	{0}