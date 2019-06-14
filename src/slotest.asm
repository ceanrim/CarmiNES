	;; I made a lot of those files to test the CPU's handling of the various
	;; instructions, when I started saving them I could only find this
	;; I write test roms directly in machine code,
	;; I'm only providing the source code for completeness but don't expect
	;; these to be in any standard format
	;; ==============================================================================
	;; Mapper: 0
	;; CHR ROM: 0K
	;; PRG ROM: 32K
	;; This cannot handle interrupts
	.code
$8000:	lda #$40
	sta $00
	slo $00
	lda #$40
	sta $03
	ldx #$03
	slo $00, x
	lda #$40
	sta $6000  		;Pls ignor these
	slo $6000               ;
	lda #$40
	sta $6003		;
	slo $6000, x		;
	lda #$40
	ldy #$05
	sta $6005		;
	slo $6000, y		;
	lda #$00
	sta $03
	lda #$60		;
	sta $04
	lda #$40
	slo ($00, x)
	lda #$00
	sta $00
	lda #$60
	sta $01
	lda #$40
	sta $6005		;
	slo ($00), y
