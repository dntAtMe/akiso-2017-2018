global find_primes
extern printf

section .text

find_primes:
	;prologue
	push 	ebp
	mov 	ebp, esp
	push 	ebx
	push	esi
	push 	edi
	
	mov 	ebx, [ebp + 8]
	mov	esi, 1
	mov 	edi, 2
find_primes_loop:
	add	esi, 1
	cmp	esi, ebx ;loop start cuz im confused
	je	find_primes_out
	push 	esi
	call	is_prime
	add	esp, 4
	
	cmp	eax, 0
	je 	find_primes_loop	


find_primes_print:
	mov	ecx, edi
	sub	edi, esi
	cmp 	edi, -2
	mov	edi, esi
	jne	find_primes_loop

	push 	esi
	push 	ecx
	push 	msg
	call 	printf
	add	esp, 12
	mov	edi, esi
	jmp	find_primes_loop

find_primes_out:
	pop	edi
        pop	esi
	pop     ebx
        mov     esp, ebp
        pop     ebp
        ret

is_prime:
	;PROLOGUE NUMERO DUO
	push 	ebp
	mov	ebp, esp
	
	push	ebx

	mov	ebx, [ebp + 8]
	mov 	ecx, 2

is_prime_loop:	
 	cmp 	ebx, ecx
	je 	is_prime_true
	
	xor	edx, edx
	mov	eax, ebx
	div 	ecx    ;remainder stored in EDX
	cmp	edx, 0
	je	is_prime_false
	;check here	

	mov 	eax, ecx
	add 	ecx, 1	
	jmp 	is_prime_loop

is_prime_true:
	mov 	eax, 1

	jmp 	is_prime_out
is_prime_false:
	mov 	eax, 0

is_prime_out:
	;epilogue
	pop	ebx
	mov 	esp, ebp
	pop	ebp
	ret	


section .data

msg: db "%d, %d", 10;
