.data
.balign 4
	num1: 	.word 0
	num2:	.word 0
	helper:	.word 0
	format:	.string "test"
	return: .word 0	
.text
.global main

main:
	ldr r1, addr_return
	str lr, [r1]	

	ldr 	r0, addr_num1
	bl	gets

	ldr 	r0, addr_num2
	bl 	gets
	
	ldr r0, addr_num1
	ldr r1, addr_num2
	ldr r1, [r1]
loop:
	cmp r1, [r0]
	beq end
	cmp [r0], r1
	bgt sub_froma
sub_fromb:
	sub r0, [r0], r1
	b loop
sub_froma:
	sub r1, r1, [r0]
	b loop	
end:
	ldr	r0
	bl	puts
	ldr	r1, addr_return
	ldr	lr, [r1]

	mov 	pc, lr

addr_num1: 	.word num1
addr_num2: 	.word num2
addr_format:	.word format
addr_return:	.word return
addr_helper:	.word helper
