; CS 3304
; Lab 32

[list -]
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"
[list +]

%if 0
1. Implement FSM for language:
sigma = {a, b} -> accept all strings with odd num a's, even num b's
%endif

;---------------------------------------------------------------------
global  isValid:function
; HLL prototype: bool isValid(char *string);
; This function receives the offset of a null-terminated array of
; characters. The function returns true if the string is valid and
; false otherwise.	
; Implements the following code:
;      let n = the length of the string
;      let state = 0
;      for (i = 0; i < n; ++i)
;      {
;        print state and array[i]
;        switch (state)
;        {
;          case 0: if array[i] == 'a'
;                    state = 1
;                  else if array[i] == 'b'
;                    state = 2
;                  else
;                    state = 4
;          case 1: if array[i] == 'a'
;                    state = 0
;                  else if array[i] == 'b'
;                    state = 3
;                  else
;                    state = 4
;          case 2: if array[i] == 'a'
;                    state = 3
;                  else if array[i] == 'b'
;                    state = 0
;                  else
;                    state = 4
;          case 3: if array[i] == 'a'
;                    state = 2
;                  else if array[i] == 'b'
;                    state = 1
;                  else
;                    state = 4
;          case 4: do nothing
;        } // end switch
;        print state
;      } // end for
;      return state == 1
; Receives: ESI = starting offset of array
; Returns:  bool in eax
;---------------------------------------------------------------------

isValid:				; IS VALID FUNCTION

SECTION .bss			; uninitialized data
n		resd	1
state	resd	1

SECTION .data
hrule	times	30	db '-'
		db		`\n`, 0
		
head1	db		`	Current		Input		Next\n	`
		db		`	State		Symbol		State\n	`, 0

SECTION .text
		pushad							; saves the 32-bit GP reg to sys stack
		
		push		esi					; place the address of string on sys stack
		call		Str_length			; should have and return len stored in A reg
		mov			[n], eax
		add			esp, 4				; cleaning the stack (remove one parameter)
		mov			esi, n
		mov			ecx, 1
		mov			ebx, 4
		; call		DumpMem
		
		mov			dword[state], 0		; set state = 0
		mov			edx, hrule
		call		WriteString
		
		mov			edx, head1
		call		WriteString
		
		mov			edx, hrule
		call		WriteString
		
		; long loop goes here
		
		mov			edx, hrule
		call		WriteString
		
		; eax = state == 1
		
		cmp			dword[state], 1
		jne			false
		mov			eax
		jmp			return
false:
		mov			eax, 0
return:	
		popad							; restore 32-bit GP reg from sys stack
		ret

;---------------------------------------------------------------------
