; CS 3304 - Lab 12

; gcc -m32 -c lab12main.char
; nasm -f elf32 -o lab12.o lab12.asm
; gcc -m32 -o lab12 lab12main.o lab12.of; ./lab122 < 00.dat
;---------------------------------------------------------------------
global  isValid:function
; Returns true if its argument is valid and false otherwise.        
; HLL prototype: int isValid(char *s);
; Implements the following HLL function:
;      int isValid(char *s)
;      {
;        int sum = 0;
;        for ( ; *s != '\0'; ++s)
;        {
;          if (*s < 'a' || *s > 'b')
;             return 0;
;          if (*s == 'a' && *(s + 1) != '\0' && *(s + 1) == 'a')
;            sum += 1;
;        }
;        return sum % 2 != 0;
;      }
; Receives: one 32-bit pointer containing the address of a string
;           via the system stack
; Returns:  a 32-bit integer in EAX
;---------------------------------------------------------------------
isValid: 
SECTION .text
        push    ebp
        mov     ebp,esp
        sub     esp,4              ; create space on stack for one 32-bit int
        pushad                     ; push eax, ecx, edx, ebx, esp, ebp, esi, edi
; Stack looks like
;
;       | param *s    |   [ebp + 8]
;       | return addr |   [ebp + 4]
;       | ebp         |   ebp
;       | local var   |   [ebp -  4] sum from above pseudocode
;       | eax         |   [ebp -  8] from pushad
;       | ecx         |   [ebp - 12] from pushad
;       | edx         |   [ebp - 16] from pushad
;       | ebx         |   [ebp - 20] from pushad
;       | esp         |   [ebp - 24] from pushad
;       | ebp         |   [ebp - 28] from pushad
;       | esi         |   [ebp - 32] from pushad
;       | edi         |   [ebp - 36] from pushad
;      
        mov     dword [ebp-4],0    ; clear accumulator; sum in pseudocode
        mov     esi,[ebp+8]        ; place addr of param in esi
        
;
whileLoop:
	mov			al, [esi]		; load s
	cmp			al, 0
	je			doneLoop		; if \0, exit
	
	; check if s<a or s>b
	cmp			al, 'a'
	jl			invalid
	cmp			al, 'b'
	jg			invalid
	
	; check if s==a
	cmp			al, 'a'
	jne			getNext
	
	; check if s+1
	mov			bl, [esi+1]
	cmp			bl, 0
	je			getNext
	cmp			bl, 'a'
	jne			getNext
	
	; found aa
	inc			dword[ebp-4]
	
getNext:
	inc			esi
	jmp			whileLoop
	
invalid:
	mov			dword[ebp-4], 0
	jmp			endValid
	
doneLoop:
	; sum%2 != 0
	mov			eax, [ebp-4]
	and			eax, 1			; eax = sum%2
	mov			[ebp-4], eax
	
endValid:
;	

        popad                      ; pop edi, esi, ebp, esp, ebx, edx, ecx, eax
        mov     eax,[ebp-4]        ; move local accumulator to eax
        add     esp,4              ; clean local var from stack
        pop     ebp
        ret
