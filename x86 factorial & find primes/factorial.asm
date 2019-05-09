



SECTION .data
	newline:	db "\n", 0
	formatn:	db "%d", 0


SECTION .bss
	n		resd 1	
	number		resd 4
SECTION .text

extern printf
extern scanf

global main

main:

	mov 	dword[number], 1
	mov 	dword[number+4], 0
	mov 	dword[number+8], 0
	mov 	dword[number+12], 0
	
	push 	n
	push 	formatn
	call 	scanf
	add	esp, 8
loop:
	cmp 	dword[n], 1
	jle	print
	call	multiplication
	sub	dword[n], 2
	jmp 	loop




multiplication:
	mov	ebx, [n]
	mov	eax, [number]
	mul	ebx
	mov 	[number], eax
	mov 	ecx, edx

	mov	eax, [number+4]
	mul	ebx
	add	eax, ecx
	mov 	[number+4], eax
	mov	ecx, edx
	
	mov 	eax, [number+8]
	mul	ebx
	add	eax, ecx
	mov	[number+8], eax
	mov 	ecx, edx

	mov 	eax, [number+12]
	mul 	ebx
	add 	eax,ecx
	mov 	[number+12], eax
	
	ret

print:
	push dword[number+12]
	push formatn
	call printf
	add ESP, 8
	      push dword[number+8]
        push formatn
        call printf
        add ESP, 8

      push dword[number+4]
        push formatn
        call printf
        add ESP, 8

      push dword[number]
        push formatn
        call printf
        add ESP, 8


	
