	opt f-g-h+l+o+
	org $1000

start	equ *

	lda <text
	sta $80
	lda >text
	sta $81
	ldy #1
	lda #24
	jsr tohex
	
	lda <text
	ldx >text
	jsr $ff80
	brk



tohex	
	pha
	jsr conv
	pla
	lsr @
	lsr @
	lsr @
	lsr @

conv
	and #%00001111
	ora #'0'
	cmp #'9'+1
	bcc store
	clc
	adc #'A'-'9'-1
store
	sta ($80),y
	dey
	rts



text
	dta b(0),b(0)
	dta b(10)
	dta b(0)


	org $2e0
	dta a(start)
