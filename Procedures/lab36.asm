%if 0
A cleric developed a procedure for determining the date of Easter.

Divide 						by 			Quotient 		Remainder
the year x 					19 							a
the year x 					100			b				c
b 							4 			d 				e
b + 8 						25 			f
b − f + 1 					3 			g
19a + b − d − g + 15 		30 							h
c 							4 			i 				k
32 + 2e + 2i − h − k 		7 							l
a + 11h + 22l 				451 		m
h + l − 7m + 114 			31 			n 				p

%endif

; ------------------------------------------------------------------------------------ ;

%if 0
THEN:

n = number of the month (3 = March, 4 = April)
	,
p + 1 = day of that month upon which Easter Sunday falls

-> valid method for all years later than 1582 in the Gregorian calendar
%endif

; ------------------------------------------------------------------------------------ ;

[list -]
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"
[list +]

;---------------------------------------------------------------------
global  EasterSunday:function
; HLL prototype: 
;   void EasterSunday(uint year);
; Receives an unsigned 32-bit integer representing a year.
; Determines the date of Easter Sunday in year. The date is written
; to stdout.        
; Receives: EAX = year
; Returns:  Nothing
;---------------------------------------------------------------------

EasterSunday:
	SECTION .data
		marchStr	db		"March", 0
		aprilStr	db		"April", 0
		
	SECTION .bss
		year    resd 1
		a       resd 1
		b       resd 1
		c       resd 1
		d       resd 1
		e       resd 1
		f       resd 1
		g       resd 1
		h       resd 1
		i       resd 1
		k       resd 1
		l       resd 1
		m       resd 1
		n       resd 1
		p       resd 1
		
	SECTION .text
		pushad
		
		; Easter algorithm
		mov [year], eax

		; a = year % 19
		xor edx, edx
		mov ecx, 19
		div ecx
		mov [a], edx

		; year /100 = b remainder c
		mov eax, [year]
		xor edx, edx
		mov ecx, 100
		div ecx
		mov [b], eax
		mov [c], edx

		; b /4 = d remainder e
		mov eax, [b]
		xor edx, edx
		mov ecx, 4
		div ecx
		mov [d], eax
		mov [e], edx

		; f = (b+8)/25
		mov eax, [b]
		add eax, 8
		xor edx, edx
		mov ecx, 25
		div ecx
		mov [f], eax

		; g = (b-f+1)/3
		mov eax, [b]
		sub eax, [f]
		add eax, 1
		xor edx, edx
		mov ecx, 3
		div ecx
		mov [g], eax

		; h = (19a+b-d-g+15)%30
		mov eax, [a]
		imul eax, 19
		add eax, [b]
		sub eax, [d]
		sub eax, [g]
		add eax, 15
		xor edx, edx
		mov ecx, 30
		div ecx
		mov [h], edx

		; c /4 = i remainder k
		mov eax, [c]
		xor edx, edx
		mov ecx, 4
		div ecx
		mov [i], eax
		mov [k], edx

		; l = (32 +2e +2i -h -k)%7
		mov eax, 32
		mov ebx, [e]
		add eax, ebx
		add eax, ebx
		mov ebx, [i]
		add eax, ebx
		add eax, ebx
		sub eax, [h]
		sub eax, [k]

		xor edx, edx
		mov ecx, 7
		div ecx
		mov [l], edx

		; m = (a +11h +22l)/451
		mov eax, [a]

		mov ebx, [h]
		imul ebx, 11
		add eax, ebx

		mov ebx, [l]
		imul ebx, 22
		add eax, ebx

		xor edx, edx
		mov ecx, 451
		div ecx
		mov [m], eax

		; (h+l-7m+114)/31 = n remainder p
		mov eax, [h]
		add eax, [l]

		mov ebx, [m]
		imul ebx, 7
		sub eax, ebx

		add eax, 114

		xor edx, edx
		mov ecx, 31
		div ecx

		mov [n], eax
		mov [p], edx

	; -------------------------------
	; 		   PRINT OUTPUT
	; -------------------------------

		; print 2 leading spaces before the year
		mov al, ' '
		call WriteChar
		mov al, ' '
		call WriteChar
		
		; print year width 6
		mov eax, [year]
		call WriteDec

		; spacing
		mov al, ' '
		call WriteChar
		mov al, ' '
		call WriteChar
		mov al, ' '
		call WriteChar
		mov al, ' '
		call WriteChar
		mov al, ' '
		call WriteChar
		mov al, ' '
		call WriteChar
		mov al, ' '
		call WriteChar

		; month
		mov eax, [n]
		cmp eax, 3
		je printMarch

	printApril:
		mov edx, aprilStr
		call WriteString
		jmp printDay

	printMarch:
		mov edx, marchStr
		call WriteString

	printDay:
		mov al,' '
		call WriteChar

		mov eax, [p]
		inc eax
		call WriteDec
		
		mov al, 10
		call WriteChar
		
		popad
		ret
		
; check Win chars -> cat -vte my.out