%if 0

1) if u and v int (not both 0), gcd(u,v) is largest > 0 int evenly dividing
2) if u and v both 0, gcd(0,0) = 0
3) if either is 0, gcd(u,0) = |u| & gcd(0,v) = |v|
4) use Euclid's algorithm to determine gcd

%endif
; ------------------------------------------------------------------------------------ ;

[list -]
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"
[list +]

;---------------------------------------------------------------------
global  GCD:function
; HLL prototype: int GCD(int x, int y);
; Returns the greatest common divisor (gcd) of two signed 32-bit
; integers.     
; Implements the following pseudocode:
;      int GCD(int x, int y)
;      {
;        x = abs(x);
;        y = abs(y);
;        if (x == 0)
;          return y;
;        else if (y == 0)
;          return x;
;        do
;        {
;          int n = x % y;
;          x = y;
;          y = n;
;        } while (y > 0);
;        return x;
;      }
; Receives: two signed 32-bit integers via the system stack
; Returns:  a 32-bit integer in EAX
;---------------------------------------------------------------------

GCD:
	SECTION .text
		; n is a local var
		push	ebp
		mov		ebp, esp
		push	ebx
		push	edx
		
		mov		eax, [ebp+8]	; x
		mov		ebx, [ebp+12]	; y
		
		; -- abs(x) --
		cmp		eax, 0
		jge		abs_y
		neg		eax
		
		; -- abs(y) --
		abs_y:
			cmp		ebx, 0
			jge		check_x
			neg		ebx
			
		; -- check for 0 --
		check_x:
			; if x==0, return y
			cmp		eax, 0
			jne		check_y
			mov		eax, ebx
			jmp		done
			
		check_y:
			; if y==0, return x
			cmp		ebx, 0
			jne		gcd_loop
			jmp		done
			
		; -- Euclid loop --
		gcd_loop:
			mov		edx, 0
			div		ebx			; quotient eax, remain edx
			
			mov		eax, ebx	; x=y
			mov		ebx, edx	; y=remain
			
			cmp		ebx, 0
			jne		gcd_loop
			
		done:
			pop 	edx
			pop		ebx
			pop		ebp
			ret
		
;---------------------------------------------------------------------
; to run:
; ../irvine_test.sh lab37 01.dat > my.out
; diff 01.out my.out