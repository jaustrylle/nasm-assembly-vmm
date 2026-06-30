; CS 3304
; Lab 33

%if 0
1. implement bubble sort for array of signed 32-bit int
2. construct 3 funcs (fill array, bubble sort (ascending), display elem)
%endif

[list -]
%INCLUDE "Along32.inc"
%INCLUDE "Macros_Along.inc"
[list +]

;---------------------------------------------------------------------
global  FillArray:function       
; HLL prototype: void FillArray(int *array, int n);
; Reads n signed 32-bit integers and stores them in array.
; Implements the following loop: 
;      for (i = 0; i < n; i++)
;      {
;        Read a signed 32-bit integer from standard input
;        Assign the acquired integer to array[i]
;      }
; Receives: ESI = starting offset of array
;           ECX = # of elements in array
; Returns:  nothing
;---------------------------------------------------------------------
FillArray:
	SECTION .text
		pushad
		
	fillLoop:
		call		ReadInt
		mov			[esi], eax		; store in array
		add			esi, 4			; ptr to next elem
		loop		fillLoop
		
		popad
		ret
;---------------------------------------------------------------------
global  BubbleSort:function
; HLL prototype: void BubbleSort(int *array, int n);
; Implements the following code:
;      for (i = 0; i < n - 1; ++i)
;      {
;        for (j = 0; j < n - i - 1; ++j)
;        {
;          if (array[j] > array[j+1])
;            swap(array[j], array[j+1])
;        }
;        DisplayArray(array, n)
;      }            
; Receives: ESI = starting offset of array
;           ECX = # of elements in array
; Returns:  nothing
;---------------------------------------------------------------------
BubbleSort:
    SECTION .text
        pushad
        ; If array size <= 1, nothing to sort
        cmp ecx, 1
        jle finish

        mov edi, 0          ; i = 0 (outer loop counter)

outer_loop:
        mov ebx, 0          ; j = 0 (inner loop index)

inner_loop:
        ; Calculate limit: eax = n - i - 1
        mov eax, ecx
        sub eax, edi
        dec eax

        cmp ebx, eax        ; if j >= (n - i - 1), exit inner
        jge after_inner

        ; Compare array[j] and array[j+1]
        mov edx, [esi + ebx*4]       ; edx = array[j]
        mov eax, [esi + ebx*4 + 4]   ; eax = array[j+1]

        cmp edx, eax
        jle no_swap         ; if array[j] <= array[j+1], no swap

        ; Swap elements
        mov [esi + ebx*4], eax
        mov [esi + ebx*4 + 4], edx

no_swap:
        inc ebx
        jmp inner_loop

after_inner:
        pushad 
        call DisplayArray
        popad

        inc edi             ; i++
        mov eax, ecx
        dec eax             ; limit = n - 1
        cmp edi, eax        ; if i < n - 1, loop again
        jl  outer_loop

finish:
        popad
        ret

;---------------------------------------------------------------------
global  DisplayArray:function
; HLL prototype: void DisplayArray(int *array, int n);
; Implements the following loop:
;      for (i = 0; i < n; ++i)
;        print array[i]
; The elements are displayed on a single line, with elements being
; separated by commas.
; Receives: ESI = starting offset of array
;           ECX = # of elements in array
; Returns:  nothing
;---------------------------------------------------------------------
DisplayArray:
    SECTION .text
        pushad
        mov ebx, ecx        ; Current count remaining
        mov edi, esi        ; Use EDI to iterate

    printLoop:
        mov eax, [edi]      ; Get the integer
        call WriteInt       ; Print the number (e.g., +10)

        dec ebx             ; Decrease count
        jz done             ; If ebx is 0, we are at the last element; EXIT

        ; Windows-style carriage return, no Crlf
        mov al, ','
        call WriteChar

        add edi, 4          ; Move to next 4-byte integer
        jmp printLoop       ; Repeat

    done:
        mov	al, 10			; ASCII for line feed of \necessarily
		call WriteChar
		
        popad
        ret

; to run: ../irvine_test.sh lab33 01.dat (1-3) > my.out 
; diff 01.out (1-3) my.out